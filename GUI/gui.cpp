#include "gui.h"

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "../utils/types.h"
#include "../sceneSetUp/volume.h"
#include "../utils/material.h"
#include "../simulations/simulations.h"

// Converts it such that (0,0) cartesian represents the middle of the screen
// hence (0,0) -> (windowX/2, windowY/2), also each pixel is 1mm, the simulation components are in cm.
void convToCameraCoords(const size_t windowX, const size_t windowY, RenderInfo& info) {
    // convert these to mm
    info.x *= 10;
    info.y *= 10;
    info.width *= 10;
    info.height *= 10;

    const int centreX{ static_cast<int>(windowX) / 2 };
    const int centreY{ static_cast<int>(windowY) / 2 };

    info.x += centreX;
    info.y = centreY - info.y;

    std::cout << "x: " << info.x <<'\n';
}

sf::Color materialToColor(const Material& m) {
    switch (m.getMaterialType()) {
        case WATER:    return sf::Color::Blue;
        case LEAD:    return sf::Color::Yellow;
        case GRAPHITE: return sf::Color(192, 192, 192);  // light gray
        default: return sf::Color::White;
    }
}

void GUI::setUp(const std::vector<const Volume*>& scene, const std::vector<Material>& materials) const {

    sf::RenderWindow window(sf::VideoMode(windowX, windowY), "My 2D Scene");
    window.setFramerateLimit(0);
    std::vector<std::unique_ptr<sf::Shape>> shapes;
    sf::Clock clock;
    const sf::Time frameDuration = sf::seconds(1.f / 30.f);

    size_t counter{};
    for (const auto obj : scene) {
        auto info { obj->renderInfo() };
        convToCameraCoords(windowX, windowY, info);

        // structured binding for readability
        const auto [type, width, height, x, y] = info;

        if (type == ShapeType::CIRCLE) {
            auto circle = std::make_unique<sf::CircleShape>(width);
            circle->setOrigin(width, width);
            circle->setPosition(x, y);
            circle->setFillColor( materialToColor(materials[counter]) );
            shapes.push_back(std::move(circle));
        } else if (type == ShapeType::SLAB) {
            auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));
            rect->setOrigin(width / 2.f, height / 2.f);
            rect->setPosition(x, y);
            rect->setFillColor( materialToColor(materials[counter]) );
            shapes.push_back(std::move(rect));
        }
        counter++;
    }

    Simulation sim(500, materials, scene);
    sim.isotropicNeutronDirections();

    std::vector neutronPositions(500, TwoVec{0.0, 0.0});
    std::vector<char> alive(500, 1);

    constexpr float neutronRadius = 3.0f; // pixels
    sf::CircleShape neutronShape(neutronRadius);
    neutronShape.setOrigin(neutronRadius, neutronRadius);
    neutronShape.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        clock.restart();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // Draw all scene shapes
        for (const auto& shape : shapes)
            window.draw(*shape);

        // Draw neutrons
        for (std::size_t i = 0; i < neutronPositions.size(); ++i) {
            if (alive[i]) {
                auto& pos = neutronPositions[i];
                const float cameraX = pos.x * 10 + 200;
                const float cameraY = 200 - pos.y * 10;

                neutronShape.setPosition( cameraX, cameraY );
                window.draw(neutronShape);
            }
        }

        window.display();

        // Advance simulation and get results
        sim.step();
        neutronPositions = sim.getNeutronPositions();
        alive = sim.getAliveNeutrons();

        // --- Frame limiting ---
        sf::Time elapsed = clock.getElapsedTime();

        std::cout<< "FPS: " << 1.0 / elapsed.asSeconds() << '\n';
        if (elapsed < frameDuration)
            sf::sleep(frameDuration - elapsed);
    }

}

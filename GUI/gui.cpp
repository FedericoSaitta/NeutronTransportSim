#include "gui.h"

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "../utils/types.h"
#include "../sceneSetUp/volume.h"

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

void GUI::setUp(const std::vector<const Volume*>& scene) const {

    sf::RenderWindow window(sf::VideoMode(windowX, windowY), "My 2D Scene");
    std::vector<std::unique_ptr<sf::Shape>> shapes;

    for (const auto obj : scene) {
        auto info { obj->renderInfo() };
        convToCameraCoords(windowX, windowY, info);

        // structured binding for readability
        const auto [type, width, height, x, y] = info;

        if (type == ShapeType::CIRCLE) {
            auto circle = std::make_unique<sf::CircleShape>(width);
            circle->setOrigin(width, width);
            circle->setPosition(x, y);
            circle->setFillColor(sf::Color::Green);
            shapes.push_back(std::move(circle));
        } else if (type == ShapeType::SLAB) {
            auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));
            rect->setOrigin(width / 2.f, height / 2.f);
            rect->setPosition(x, y);
            rect->setFillColor(sf::Color::Blue);
            shapes.push_back(std::move(rect));
        }
    }

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // Draw all shapes
        for (const auto& shape : shapes)
            window.draw(*shape);

        window.display();
    }

}

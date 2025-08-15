#include "gui.h"

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "../utils/types.h"
#include "../sceneSetUp/volume.h"

void convToCameraCoords(RenderInfo& info) {
    info.x *= 10;
    info.y *= 10;
    info.width *= 10;
    info.height *= 10;
}

void GUI::setUp(const std::vector<const Volume*>& scene) const {

    sf::RenderWindow window(sf::VideoMode(windowX, windowY), "My 2D Scene");
    std::vector<std::unique_ptr<sf::Shape>> shapes;

    // structured binding for better readability
    for (const auto obj : scene) {
        auto info { obj->renderInfo() };
        convToCameraCoords(info);

        const auto [type, width, height, x, y] = info;



        if (type == ShapeType::CIRCLE) {
            auto circle = std::make_unique<sf::CircleShape>(width);
            circle->setPosition(x, y);
            circle->setFillColor(sf::Color::Green);
            shapes.push_back(std::move(circle));
        }

        else if (type == ShapeType::SLAB) {
            auto rect = std::make_unique<sf::RectangleShape>(sf::Vector2f(width, height));
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

#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

#include "../sceneSetUp/volume.h"
#include "../utils/material.h"

inline sf::Color materialToColor(const Material& m) {
    switch (m.getMaterialType()) {
        case WATER:    return sf::Color::Blue;
        case LEAD:    return sf::Color::Yellow;
        case GRAPHITE: return sf::Color(192, 192, 192);  // light gray
        default: return sf::Color::White;
    }
}

// Converts it such that (0,0) cartesian represents the middle of the screen
// hence (0,0) -> (windowX/2, windowY/2), also each pixel is 1mm, the simulation components are in cm.
inline void convToCameraCoords(const size_t windowX, const size_t windowY, RenderInfo& info) {
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


class GUI {
public:
    GUI() : windowX(100), windowY(100) {}
    GUI(const size_t xDim, const size_t yDim) : windowX(xDim + 200), windowY(yDim) {
        init();
    }


    void setUp(const std::vector<const Volume*>& scene, const std::vector<Material>& materials);
    void init() {
        if (!font.loadFromFile("../arial.ttf"))
            throw std::runtime_error("Could not load font");
    }

    void setUpScene(const std::vector<const Volume*>& scene,  const std::vector<Material>& materials) {
        size_t counter{};
        for (const auto obj : scene) {
            auto info { obj->renderInfo() };
            // Use the simulation area width instead of full window width
            convToCameraCoords(windowX-200, windowY, info);

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
    }


private:
    size_t windowX;
    size_t windowY;
    sf::Font font;
    std::vector<std::unique_ptr<sf::Shape>> shapes;
};
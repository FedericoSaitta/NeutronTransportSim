#include "gui.h"

#include <vector>
#include <memory>
#include <sstream>
#include <SFML/Graphics.hpp>

#include "../utils/types.h"
#include "../sceneSetUp/volume.h"
#include "../utils/material.h"
#include "../simulations/simulations.h"


void GUI::setUp(const std::vector<const Volume*>& scene, const std::vector<Material>& materials) {

    sf::RenderWindow window(sf::VideoMode(windowX, windowY), "My 2D Scene");
    window.setFramerateLimit(0);
    sf::Clock clock;
    const sf::Time frameDuration = sf::seconds(1.f / 30.f);

    constexpr float panelWidth = 200.0f;
    const float simWidth = windowX - panelWidth;  // Available width for simulation

    // Create the side panel - full height, positioned at right edge
    sf::RectangleShape sidePanel(sf::Vector2f(panelWidth, windowY));
    sidePanel.setFillColor(sf::Color(128, 128, 128, 255)); // Solid gray (not transparent)
    sidePanel.setPosition(windowX - panelWidth, 0); // Right edge, full height

    // Adjust the info text position to be inside the panel
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(14);
    infoText.setFillColor(sf::Color::Black); // Black text on gray background
    infoText.setPosition(windowX - panelWidth + 20, 20); // 20px padding from panel edge

    setUpScene(scene, materials);

    Simulation sim(50000, materials, scene);
    sim.isotropicNeutronDirections();
    std::vector neutronPositions(50000, TwoVec{0.0, 0.0});
    std::vector<char> alive(50000, 1);

    constexpr float neutronRadius = 3.0f; // pixels
    sf::CircleShape neutronShape(neutronRadius);
    neutronShape.setOrigin(neutronRadius, neutronRadius);
    neutronShape.setFillColor(sf::Color::White);

    // Create the main simulation view - this will be constrained to the simulation area
    sf::View simView(sf::FloatRect(0, 0, simWidth, windowY));
    simView.setCenter(simWidth / 2.0f, windowY / 2.0f);  // Center on (0,0) in world coords
    simView.setViewport(sf::FloatRect(0.0f, 0.0f, simWidth / windowX, 1.0f)); // Only use left portion

    sf::Vector2f oldPos;
    bool moving = false;
    float zoom = 1;
    bool runSim = true;

    while (window.isOpen()) {
        clock.restart();

        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                break;
                case sf::Event::MouseButtonPressed:
                    // Mouse button is pressed, get the position and set moving as active
                    if (event.mouseButton.button == 0) {
                        // Only allow panning if mouse is in simulation area
                        if (event.mouseButton.x < simWidth) {
                            moving = true;
                            oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), simView);
                        }
                    }
                break;
                case sf::Event::MouseButtonReleased:
                    // Mouse button is released, no longer move
                    if (event.mouseButton.button == 0) {
                        moving = false;
                    }
                break;
                case sf::Event::MouseMoved:
                {
                    // Ignore mouse movement unless a button is pressed (see above)
                    if (!moving)
                        break;
                    // Determine the new position in world coordinates
                    const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), simView);
                    // Determine how the cursor has moved
                    // Swap these to invert the movement direction
                    const sf::Vector2f deltaPos = oldPos - newPos;

                    // Move our view accordingly and update the window
                    simView.setCenter(simView.getCenter() + deltaPos);

                    // Save the new position as the old one
                    // We're recalculating this, since we've changed the view
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), simView);
                    break;
                }
                case sf::Event::MouseWheelScrolled:
                {
                    // Ignore the mouse wheel unless we're not moving and mouse is in sim area
                    if (moving || event.mouseWheelScroll.x >= simWidth)
                        break;

                    // Determine the scroll direction and adjust the zoom level
                    // Again, you can swap these to invert the direction
                    if (event.mouseWheelScroll.delta <= -1)
                        zoom = std::min(2.f, zoom + .1f);
                    else if (event.mouseWheelScroll.delta >= 1)
                        zoom = std::max(.5f, zoom - .1f);

                    // Update our view
                    simView.setSize(sf::Vector2f(simWidth, windowY)); // Reset the size to sim area
                    simView.zoom(zoom); // Apply the zoom level (this transforms the view)
                    break;
                }
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Space)
                        runSim = !runSim;
                break;

                default: break;
            }
        }

        window.clear(sf::Color::Black);

        // Set the simulation view for drawing the main scene
        window.setView(simView);

        // Draw all scene shapes
        for (const auto& shape : shapes)
            window.draw(*shape);

        // Draw neutrons
        for (std::size_t i = 0; i < neutronPositions.size(); ++i) {
            if (alive[i]) {
                auto& pos = neutronPositions[i];
                // Adjust neutron positioning for the simulation area
                const float cameraX = pos.x * 10 + simWidth / 2.0f;
                const float cameraY = windowY / 2.0f - pos.y * 10;

                neutronShape.setPosition(cameraX, cameraY);
                window.draw(neutronShape);
            }
        }

        // Get centre of the current simulation view
        sf::Vector2f centre = simView.getCenter();

        // Build info string
        std::ostringstream ss;
        ss << "Zoom: " << zoom << "\n"
           << "Centre: (" << centre.x << ", " << centre.y << ")\n"
           << "Running: " << (runSim ? "Yes\n" : "No\n")
           << "Absorbed: " << sim.getNumAbsorbed() << '\n';

        infoText.setString(ss.str());

        // Switch to the default view for UI elements (side panel)
        window.setView(window.getDefaultView());

        // Draw UI elements
        window.draw(sidePanel);
        window.draw(infoText);

        window.display();

        if (runSim) {
            // Advance simulation and get results
            sim.step();
            neutronPositions = sim.getNeutronPositions();
            alive = sim.getAliveNeutrons();
        }

        // --- Frame limiting ---
        sf::Time elapsed = clock.getElapsedTime();

        // std::cout<< "FPS: " << 1.0 / elapsed.asSeconds() << '\n';
        if (elapsed < frameDuration)
            sf::sleep(frameDuration - elapsed);
    }
}
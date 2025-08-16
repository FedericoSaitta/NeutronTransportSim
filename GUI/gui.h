#pragma once

#include <vector>
#include <SFML/Graphics.hpp>

#include "../sceneSetUp/volume.h"
#include "../utils/material.h"


class GUI {
public:
    GUI() : windowX(100), windowY(100) {}
    GUI(const size_t xDim, const size_t yDim) : windowX(xDim), windowY(yDim) {}


    void setUp(const std::vector<const Volume*>& scene, const std::vector<Material>& materials) const;



private:
    size_t windowX;
    size_t windowY;





};
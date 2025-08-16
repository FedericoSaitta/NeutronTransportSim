#pragma once

#include <limits>
#include "types.h"

class Material{
public:
    // Creates a 'Vacuum' material
    Material() : crossSection(0.0), absorptionProbability(0.0),
        meanFreePath(std::numeric_limits<double>::max()), type(VACUUM) {}

    Material(const double crossSec, const double absProb, const MaterialTypes type) :
        crossSection(crossSec), absorptionProbability(absProb), meanFreePath(1.0 / absProb), type(type) {}

    double getCrossSec() const { return crossSection; }
    double getAbsorptionProb() const { return absorptionProbability; }
    double getMeanFreePath() const { return 1.0 / crossSection; }
    MaterialTypes getMaterialType() const { return type; }

private:
    double crossSection;
    double absorptionProbability;
    double meanFreePath;
    MaterialTypes type;

};
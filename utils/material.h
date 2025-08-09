#pragma once

#include <limits>

class Material{
public:
    // Creates a 'Vacuum' material
    Material() : crossSection(0.0), absorptionProbability(0.0),
        meanFreePath(std::numeric_limits<double>::max()) {}

    Material(const double crossSec, const double absProb) :
        crossSection(crossSec), absorptionProbability(absProb), meanFreePath(1.0 / absProb) {}

    double getCrossSec() const { return crossSection; }
    double getAbsorptionProb() const { return absorptionProbability; }
    double getMeanFreePath() const { return 1.0 / crossSection; }
private:
    double crossSection;
    double absorptionProbability;

    double meanFreePath;
};
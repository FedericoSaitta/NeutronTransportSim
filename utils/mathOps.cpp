#include "mathOps.h"

#include <random>

ThreeVec generate_isotropic_3vec(std::mt19937& gen, std::uniform_real_distribution<double>& dist) {
    const double u1 { dist(gen) };
    const double u2 { dist(gen) };

    const double phi { 2 * M_PI * u1 };
    const double theta { std::acos( 1 - 2 * u2 ) };

    const double x { std::sin(theta) * std::cos(phi) };
    const double y { std::sin(theta) * std::sin(phi) };
    const double z { std::cos(theta) };

    return {x, y, z};
}

double generate_isotropic_xcoord(std::mt19937& gen, std::uniform_real_distribution<double>& dist) {
    const double u1 { dist(gen) };
    const double u2 { dist(gen) };

    const double phi { 2 * M_PI * u1 };
    const double theta { std::acos( 1 - 2 * u2 ) };

    const double x { std::sin(theta) * std::cos(phi) };
    return x;
}

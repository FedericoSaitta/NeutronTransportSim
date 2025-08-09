// This header file will contain mathematical operations and fast approximations
#pragma once

#include <random>
#include "types.h"

// Credits to Martin Ankler:
// martin.ankerl.com/2007/10/04/optimized-pow-approximation-for-java-and-c-c/
inline double fastLog(double x) {
    union { double d; long long l; } u = { x };
    return (u.l - 4606931270219946880LL) * 1.539095918623324e-16;
}

inline ThreeVec generate_isotropic_3vec(std::minstd_rand& gen, std::uniform_real_distribution<double>& dist) {
    const double u1 { dist(gen) };
    const double u2 { dist(gen) };

    const double phi { 2 * M_PI * u1 };
    const double theta { std::acos( 1 - 2 * u2 ) };

    const double x { std::sin(theta) * std::cos(phi) };
    const double y { std::sin(theta) * std::sin(phi) };
    const double z { std::cos(theta) };

    return {x, y, z};
}

inline double generate_isotropic_xcoord(std::minstd_rand& gen, std::uniform_real_distribution<double>& dist) {
    const double u1 { dist(gen) };
    const double u2 { dist(gen) };

    const double phi { 2 * M_PI * u1 };
    const double theta { std::acos( 1 - 2 * u2 ) };

    const double x { std::sin(theta) * std::cos(phi) };
    return x;
}

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

inline TwoVec generate_isotropic_2vec(std::minstd_rand& gen, std::uniform_real_distribution<double>& dist) {
    const double angle = 2.0 * M_PI * dist(gen);
    const double x = std::cos(angle);
    const double y = std::sin(angle);
    return {x, y};
}

inline double generate_isotropic_xcoord(std::minstd_rand& gen, std::uniform_real_distribution<double>& dist) {
    const double angle = 2.0 * M_PI * dist(gen);
    const double x = std::cos(angle);
    return x;
}

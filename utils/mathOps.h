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

ThreeVec generate_isotropic_3vec(std::mt19937& gen, std::uniform_real_distribution<double>& dist);
double generate_isotropic_xcoord(std::mt19937& gen, std::uniform_real_distribution<double>& dist);
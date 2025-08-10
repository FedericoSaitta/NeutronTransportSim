#include <iostream>
#include <random>

#include "utils/mathOps.h"
#include "utils/timer.h"
#include "utils/material.h"
#include "simulations/simulations.h"

static std::random_device rd;
static std::mt19937 gen(rd()); // Mersenne Twister generator
static std::uniform_real_distribution<double> dist(0.0, 1.0);

// Enabling optimizations enables:
// - linear instead of trig random vector (so not isotropic)
// faster log expression
int main() {
    const Material water{3.47, 0.642 / 100.0};
    const Material lead{0.38, 1.389 / 100.0};
    const Material graphite{0.40, 0.095 / 100.0};
    const Material vacuum{};

    constexpr int numNeutrons{ 500'000 };
    constexpr double slabSize{ 10.0 };

    Timer t{};
    ThreeVec results = fastSimulation<NO_OPT>(numNeutrons, water, slabSize);
    t.display();

    std::cout << "Reflected: " << results.x
              << ", Absorbed: " << results.y
              << ", Transmitted: " << results.z
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';

    std::cout << "Volume Simulation\n";

    const Slab slab(0.0, 10.0);

    t.reset();
    results = volumeSimulation(numNeutrons, water, slab);
    t.display();

    std::cout << "Reflected: " << results.x
              << ", Absorbed: " << results.y
              << ", Transmitted: " << results.z
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';

    t.reset();
    results = volumeSimulation(numNeutrons, water, slab);
    t.display();

    std::cout << "Reflected: " << results.x
              << ", Absorbed: " << results.y
              << ", Transmitted: " << results.z
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';

    return 0;
}

#include <iostream>
#include <random>

#include "utils/mathOps.h"
#include "utils/timer.h"
#include "utils/material.h"
#include "simulations/simulations.h"

#include "GUI/gui.h"

// Enabling optimizations enables:
// - linear instead of trig random vector (so not isotropic)
// faster log expression
int main() {
    const Material water{3.47, 0.642 / 100.0, WATER};
    const Material lead{0.38, 1.389 / 100.0, LEAD};
    const Material graphite{0.40, 0.095 / 100.0, GRAPHITE};
    const Material vacuum{};

    constexpr int numNeutrons{ 0 };
    constexpr double slabSize{ 10.0 };

    Timer t{};
    SimReuslts results = fastSimulation<NO_OPT>(numNeutrons, water, slabSize);
    t.display();

    std::cout << "Reflected: " << results.reflected
              << ", Absorbed: " << results.absorbed
              << ", Transmitted: " << results.transmitted
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';

    std::cout << "Volume Simulation\n";

    const Slab slab(0.0, 10.0);

    t.reset();
    results = volumeSimulation(numNeutrons, water, slab);
    t.display();

    std::cout << "Reflected: " << results.reflected
              << ", Absorbed: " << results.absorbed
              << ", Transmitted: " << results.transmitted
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';

    t.reset();
    results = volumeSimulation(numNeutrons, graphite, slab);
    t.display();

    std::cout << "Reflected: " << results.reflected
              << ", Absorbed: " << results.absorbed
              << ", Transmitted: " << results.transmitted
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';


    std::cout << "Testing out Woodcock method\n";

    const Slab slab1(0.0, 5.0);
    const Slab slab2(5.0, 10.0);

    t.reset();
    results = volumeWoodCockSimulation(numNeutrons, graphite, graphite, slab1, slab2);
    t.display();

    std::cout << "Reflected: " << results.reflected
              << ", Absorbed: " << results.absorbed
              << ", Transmitted: " << results.transmitted
              << ", kWalks/s: " << numNeutrons / t.elapsed() <<'\n';


    std::cout << "Now setting up GUI\n";
    const GUI gui{ 400, 400 };


    const Slab slab3(0.0, 5.0);
    const Slab slab5(5.0, 10.0);
    const Slab slab4(-10.0, 0.0);

    const Circle circle (2.5, 2.5, 0.0);
    const std::vector<const Volume*> scene{ &circle, &slab4, &slab5 };
    const std::vector<Material> materials{ lead, graphite, water };

    gui.setUp(scene, materials);

    return 0;
}

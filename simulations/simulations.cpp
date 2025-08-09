#include "simulations.h"

#include <vector>
#include <random>
#include "../utils/material.h"
#include "../utils/mathOps.h"
#include "../utils/types.h"

ThreeVec fastSimulation(const unsigned long numNeutrons, const Material& mat, const double slabSize) {
    size_t absorbed = 0;
    size_t transmitted = 0;
    size_t reflected = 0;

    // Pre-allocate contiguous memory
    std::vector positions(numNeutrons, 0.0);
    std::vector directions(numNeutrons, 1.0);
    std::vector<double> random_step(numNeutrons);
    std::vector<double> random_abs(numNeutrons);
    std::vector<double> random_dir(numNeutrons);

    // Random setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution dist(0.0, 1.0);

    size_t activeCount = numNeutrons;

    while (activeCount > 0) {
        // Generate all random numbers in bulk
        for (size_t i = 0; i < activeCount; ++i) {
            random_step[i] = dist(gen);
            random_abs[i] = dist(gen);
            random_dir[i] = 2 * dist(gen) - 1;// generate_isotropic_xcoord(gen, dist);
        }

        // Update positions - single pass
        for (size_t i = 0; i < activeCount; ++i) {
            positions[i] += directions[i] * -mat.getMeanFreePath() * std::log(random_step[i]);
        }

        // Compact in-place while checking conditions
        size_t newActiveCount = 0;
        for (size_t i = 0; i < activeCount; ++i) {
            const double pos = positions[i];

            if (pos <= 0.0) {
                ++reflected;
            } else if (pos >= slabSize) {
                ++transmitted;
            } else if (random_abs[i] < mat.getAbsorptionProb()) {
                ++absorbed;
            } else {
                // Keep neutron for next step
                positions[newActiveCount] = pos;
                directions[newActiveCount] = random_dir[i];  // Fast direction
                ++newActiveCount;
            }
        }

        activeCount = newActiveCount;
    }

    return {static_cast<double>(reflected),
            static_cast<double>(absorbed),
            static_cast<double>(transmitted)};
}
#include "simulations.h"

#include <vector>
#include <random>
#include "../utils/material.h"
#include "../utils/mathOps.h"
#include "../utils/types.h"
#include "../sceneSetUp/volume.h"


ThreeVec volumeSimulation(const unsigned long numNeutrons, const Material& mat, const Volume& vol) {
    size_t absorbed = 0;
    size_t transmitted = 0;
    size_t reflected = 0;

    double randomStep{};
    double randomAbsorp{};

    // Random setup
    std::random_device rd;
    std::minstd_rand gen(rd()); // Faster than mt19937
    std::uniform_real_distribution dist(0.0, 1.0);

    for (size_t i{}; i < numNeutrons; i++){
        ThreeVec neutronPosition{0.0, 0.0, 0.0};
        ThreeVec neutronDirection{1.0, 0.0, 0.0};

        bool isFirstStep{ true };

        while (true) {
            randomStep = dist(gen);
            randomAbsorp = dist(gen);

            if (isFirstStep) isFirstStep = false;
            else neutronDirection = generate_isotropic_3vec(gen, dist);

            neutronPosition = neutronPosition +  neutronDirection * -std::log(randomStep) * mat.getMeanFreePath();

            if (!vol.contains(neutronPosition)) {
                reflected++;
                break;
            }

            if (randomAbsorp < mat.getAbsorptionProb()) {
                absorbed++;
                break;
            }
        }

    }



    return {static_cast<double>(reflected),
            static_cast<double>(absorbed),
            static_cast<double>(transmitted)};
}
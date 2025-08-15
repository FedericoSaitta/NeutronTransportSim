#include "simulations.h"

#include <vector>
#include <random>
#include <tuple>

#include "../utils/material.h"
#include "../utils/mathOps.h"
#include "../utils/types.h"
#include "../sceneSetUp/volume.h"
#include "../utils/logger.h"



SimReuslts volumeSimulation(const unsigned long numNeutrons, const Material& mat, const Volume& vol) {
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
        TwoVec neutronPosition{0.0, 0.0};
        TwoVec neutronDirection{1.0, 0.0};

        bool isFirstStep{ true };

        while (true) {
            randomStep = dist(gen);
            randomAbsorp = dist(gen);

            if (isFirstStep) isFirstStep = false;
            else neutronDirection = generate_isotropic_2vec(gen, dist);

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

    return {absorbed, reflected, transmitted};
}



SimReuslts volumeWoodCockSimulation(const unsigned long numNeutrons, const Material& mat1, const Material& mat2, const Volume& vol1, const Volume& vol2) {
    size_t absorbed = 0;
    size_t reflected = 0;

    const std::vector<const Volume*> volumes { &vol1, &vol2 };
    const std::vector<Material> materials { mat1, mat2 };

    const double majorantCrossSec{ std::max(mat1.getCrossSec(),mat2.getCrossSec()) };
    const double minMeanFreePath{ 1.0 / majorantCrossSec };

    // Random setup
    std::random_device rd;
    std::minstd_rand gen(rd()); // Faster than mt19937
    std::uniform_real_distribution dist(0.0, 1.0);

    for (size_t i{}; i < numNeutrons; i++) {
        TwoVec neutronPosition{0.0, 0.0};
        TwoVec neutronDirection{1.0, 0.0};

        // First step is never fictitious
        bool isStepFict{ false };

        // First step performed outside sim.
        neutronPosition = neutronPosition +  neutronDirection * -std::log( dist(gen) ) * minMeanFreePath;

        DEBUG_LOG("Neutron num: " + std::to_string(i));

        while (true) {
            double currentMeanPath{};
            double currentAbsProb{};

            double neutronLeft{ true };
            for (size_t j{}; j < volumes.size(); j++) {
                if ( volumes[j]->contains(neutronPosition) ) {
                    neutronLeft = false;
                    break;
                }
            }

            DEBUG_LOG("\tHas left: " + std::to_string(neutronLeft));

            // exit out of the loop if neutron left the system
            if (neutronLeft) {
                reflected++;
                break;
            }

            // Get the correct material variables
            for (size_t j{}; j < volumes.size(); j++) {
                if ( volumes[j]->contains(neutronPosition) ) {
                    currentMeanPath = materials[j].getMeanFreePath();
                    currentAbsProb = materials[j].getAbsorptionProb();
                    break;
                }
            }

            DEBUG_LOG("\tCurrent Mean Path: " + std::to_string(currentMeanPath));
            DEBUG_LOG("\tCurrent AbsProb: " + std::to_string(currentAbsProb));

            // only non-fictitious steps can be absorbed
            if (!isStepFict &&  dist(gen) < currentAbsProb) {

                DEBUG_LOG("\tNeutron Absorbed");
                absorbed++;
                break;
            }

            const double probFictitious{ 1.0 / (majorantCrossSec * currentMeanPath) };

            //std::cout << "probFictitious: " << probFictitious << '\n';

            if (dist(gen) > probFictitious) {
                isStepFict = true;
            }
            else {
                // change direction as step is not fictitious
                isStepFict = false;
                neutronDirection = generate_isotropic_2vec(gen, dist);
            }

            const double stepLength{ -minMeanFreePath * std::log(dist(gen)) };
            neutronPosition = neutronPosition +  neutronDirection * stepLength;

            DEBUG_LOG("\tStep Length" + std::to_string(stepLength));
        }
    }

    return {absorbed, reflected, 0};
}





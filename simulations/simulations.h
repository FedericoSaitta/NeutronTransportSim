#pragma once

#include <vector>
#include <random>
#include <algorithm>

#include "../utils/material.h"
#include "../utils/types.h"
#include "../utils/mathOps.h"
#include "../sceneSetUp/volume.h"
#include "../utils/logger.h"

SimReuslts volumeSimulation(const unsigned long numNeutrons, const Material& mat, const Volume& vol);
SimReuslts volumeWoodCockSimulation(const unsigned long numNeutrons, const Material& mat1, const Material& mat2, const Volume& vol1, const Volume& vol2);

void stepVolumeWoodCockSimulation(std::vector<TwoVec>& neutronPositions, std::vector<bool>& isStepFict, std::vector<bool>& alive,const std::vector<Material>& materials, const std::vector<const Volume*> &volumes);
template<EnableOptimizations opt>
SimReuslts fastSimulation(const unsigned long numNeutrons, const Material& mat, const double slabSize) {
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
    std::minstd_rand gen(rd()); // Faster than mt19937
    std::uniform_real_distribution dist(0.0, 1.0);

    size_t activeCount = numNeutrons;

    while (activeCount > 0) {
        // Generate all random numbers in bulk
        for (size_t i = 0; i < activeCount; ++i) {
            random_step[i] = dist(gen);
            random_abs[i] = dist(gen);
            if (opt == OPT) random_dir[i] = 2 * dist(gen) - 1;
            else random_dir[i] = generate_isotropic_xcoord(gen, dist);
        }

        // Update positions - single pass
        for (size_t i = 0; i < activeCount; ++i) {
            if (opt == OPT) positions[i] += directions[i] * -mat.getMeanFreePath() * fastLog(random_step[i]);
            else positions[i] += directions[i] * -mat.getMeanFreePath() * std::log(random_step[i]);
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
                directions[newActiveCount] = random_dir[i];
                ++newActiveCount;
            }
        }

        activeCount = newActiveCount;
    }

    return {absorbed, reflected, transmitted};
}


class Simulation {
public:
    // Intializing simulation with all alive neutrons which their first step will not be fictitious
    Simulation(const size_t numNeutrons, const std::vector<Material>& materials,
               const std::vector<const Volume*>& volumes) :
            m_materials(materials), m_volumes(volumes), m_numNeutrons(numNeutrons), m_alive(numNeutrons, 1),
            m_isStepFict(numNeutrons, 0), m_neutronPositions(numNeutrons, TwoVec{0.0, 0.0}), m_neutronDirections(numNeutrons, TwoVec{1.0, 0.0}) {
            // neutrons facing x axis by default

        m_majorantCrossSec = -1;
        for (size_t i{}; i < m_materials.size(); i++)
            m_majorantCrossSec = std::max( m_majorantCrossSec, m_materials[i].getCrossSec() );

        m_minMeanFreePath = 1.0 / m_majorantCrossSec ;
        std::uniform_real_distribution<double> jitter(-1e-6, 1e-6);
        for (auto &pos : m_neutronPositions) {
            pos.x += jitter(m_gen);
            pos.y += jitter(m_gen);
        }
    }

    // randomizes the neutron directions as in some experiments they might originate conically or isotropically
    void isotropicNeutronDirections() {
        for (size_t i{}; i < m_numNeutrons; i++)
            m_neutronDirections[i] = generate_isotropic_2vec(m_gen, m_dist);
    }


    // does one step in the simulation
    void step() {
        for (size_t i{}; i < m_numNeutrons; i++) {
            DEBUG_LOG("Neutron num: " + std::to_string(i));

            double currentMeanPath{};
            double currentAbsProb{};

            double neutronLeft{ true };
            for (size_t j{}; j < m_volumes.size(); j++) {
                if ( m_volumes[j]->contains(m_neutronPositions[i]) ) {
                    neutronLeft = false;
                    break;
                }
            }

            DEBUG_LOG("\tHas left: " + std::to_string(neutronLeft));

            // exit out of the loop if neutron left the system
            if (neutronLeft) {
                m_alive[i] = false;
                continue;
            }

            // Get the correct material variables
            for (size_t j{}; j < m_volumes.size(); j++) {
                if ( m_volumes[j]->contains(m_neutronPositions[i]) ) {
                    currentMeanPath = m_materials[j].getMeanFreePath();
                    currentAbsProb = m_materials[j].getAbsorptionProb();
                    break;
                }
            }

            DEBUG_LOG("\tCurrent Mean Path: " + std::to_string(currentMeanPath));
            DEBUG_LOG("\tCurrent AbsProb: " + std::to_string(currentAbsProb));

            // only non-fictitious steps can be absorbed
            if (!m_isStepFict[i] &&  m_dist(m_gen) < currentAbsProb) {
                DEBUG_LOG("\tNeutron Absorbed");
                m_alive[i] = false;
                continue;
            }

            const double probFictitious{ 1.0 / (m_majorantCrossSec * currentMeanPath) };

            DEBUG_LOG("\tprobFictitious: " + std::to_string(probFictitious));
            if (m_dist(m_gen) > probFictitious) {
                m_isStepFict[i] = true;
            }
            else {
                // change direction as step is not fictitious
                m_isStepFict[i] = false;
                m_neutronDirections[i] = generate_isotropic_2vec(m_gen, m_dist);
            }

            const double stepLength{ -m_minMeanFreePath * std::log(m_dist(m_gen)) };
            m_neutronPositions[i] = m_neutronPositions[i] + m_neutronDirections[i] * stepLength;

            DEBUG_LOG("\tStep Length" + std::to_string(stepLength));
        }
    }

    void printSimStats() const {
        const size_t aliveCount = std::ranges::count(m_alive, true);
        std::cout << "Number of Neutrons Alive: " << aliveCount << '/' << m_numNeutrons << '\n';
    }

    std::vector<TwoVec> getNeutronPositions() const { return m_neutronPositions; }
    std::vector<char> getAliveNeutrons() const { return m_alive; }

private:
    // Using char instead of bool because of how std::vector handles bools with proxy objects
    std::vector<Material> m_materials;
    std::vector<const Volume*> m_volumes;
    size_t m_numNeutrons;

    std::vector<char> m_alive;
    std::vector<char> m_isStepFict;
    std::vector<TwoVec> m_neutronPositions;
    std::vector<TwoVec> m_neutronDirections;

    double m_majorantCrossSec;
    double m_minMeanFreePath;

    std::minstd_rand m_gen{std::random_device{}()};
    std::uniform_real_distribution<double> m_dist{0.0, 1.0};
};




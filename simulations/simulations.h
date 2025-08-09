#pragma once

#include "../utils/material.h"
#include "../utils/types.h"

ThreeVec fastSimulation(const unsigned long numNeutrons, const Material& mat, const double slabSize);
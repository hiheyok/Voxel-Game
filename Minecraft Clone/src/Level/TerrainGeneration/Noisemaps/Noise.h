#pragma once
#include "Utils/FastNoiseLite.h"
#include "Level/TerrainGeneration/Random/JavaRandom.h"

class NoiseGenerator {
private:
    FastNoiseLite noise;

public:
    NoiseGenerator(JavaRandom seed) {
        (void)seed; //TODO Check this again later
    }
};
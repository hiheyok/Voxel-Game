#pragma once
#include "Level/TerrainGeneration/Random/JavaRandom.h"
#include "Utils/FastNoiseLite.h"

class NoiseGenerator {
   private:
    FastNoiseLite noise;

   public:
    NoiseGenerator(JavaRandom seed) {
        (void)seed;  // TODO Check this again later
    }
};
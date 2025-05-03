#pragma once
#include "Level/TerrainGeneration/Random/JavaRandom.h"
#include "Utils/FastNoiseLite.h"

class NoiseGenerator {
 private:
  FastNoiseLite noise;

 public:
  explicit NoiseGenerator(JavaRandom seed) {
    (void)seed;
  }
};

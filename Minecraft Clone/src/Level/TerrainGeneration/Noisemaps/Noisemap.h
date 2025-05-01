#pragma once
#include "Utils/FastNoiseLite.h"

class NoiseMapTypes {
 public:
  void Initialize(int SEED);

  template <typename T>
  float GetNoise(T x, T y, T z, int Octaves, float zoom_);

  FastNoiseLite IslandLayer4096;
  FastNoiseLite IslandLayer2048;
  FastNoiseLite TemperatureLayer512;
  FastNoiseLite RainfallLayer512;

 private:
  float PowTableHalf[32]{};
  float PowTable[32]{};

  FastNoiseLite OctaveNoise;

  int SEED_OFFSET = 0x93496;
};

class NoiseMaps1122 {
 public:
  FastNoiseLite minLimitPerlinNoise;
  FastNoiseLite maxLimitPerlinNoise;
  FastNoiseLite mainPerlinNoise;
  FastNoiseLite surfaceNoise;
  FastNoiseLite scaleNoise;
  FastNoiseLite depthNoise;
  FastNoiseLite forestNoise;
};

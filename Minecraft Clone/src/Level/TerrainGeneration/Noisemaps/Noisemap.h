#pragma once
#include "../../../Utils/FastNoiseLite.h"

class NoiseMapTypes {
public:
	void Initialize(int32_t SEED);

	template <typename T> float GetNoise(T x, T y, T z, int Octaves, float zoom_);

	FastNoiseLite IslandLayer4096;
	FastNoiseLite IslandLayer2048;
	FastNoiseLite TemperatureLayer512;
	FastNoiseLite RainfallLayer512;
private:

	float PowTableHalf[32]{};
	float PowTable[32]{};

	FastNoiseLite OctaveNoise;

	int32_t SEED_OFFSET = 0x93496;
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
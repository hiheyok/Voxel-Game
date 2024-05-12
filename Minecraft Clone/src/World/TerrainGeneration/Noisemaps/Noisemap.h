#pragma once
#include "../../../../../Utils/FastNoiseLite.h"

class NoiseMapTypes {
public:
	void Initialize(int32_t SEED) {
		IslandLayer4096.SetNoiseType(IslandLayer4096.NoiseType_OpenSimplex2);
		IslandLayer4096.SetFrequency(1.f);
		IslandLayer4096.SetSeed(SEED);

		IslandLayer2048.SetNoiseType(IslandLayer4096.NoiseType_OpenSimplex2);
		IslandLayer2048.SetFrequency(1.f);
		IslandLayer2048.SetSeed(SEED + SEED_OFFSET);
	}

	FastNoiseLite IslandLayer4096;
	FastNoiseLite IslandLayer2048;
private:
	int32_t SEED_OFFSET = 0x93496;
};
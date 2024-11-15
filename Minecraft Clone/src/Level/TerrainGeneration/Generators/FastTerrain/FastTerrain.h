#pragma once
#include "../Generator.h"

class FastTerrain : public WorldGenerator {
public:
	FastTerrain() {
		Noise.SetNoiseType(Noise.NoiseType_OpenSimplex2);
		Noise.SetFrequency(0.009f);
	}

	Chunk* Generate(glm::ivec3 Position) override;

	FastNoiseLite Noise;

	double TREE_RAND_VAL = 0.5;
	double TREE_RAND_VAL_RANGE = .01f;
};
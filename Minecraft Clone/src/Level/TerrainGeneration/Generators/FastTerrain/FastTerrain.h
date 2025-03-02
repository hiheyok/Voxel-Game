#pragma once
#include "../Generator.h"

class FastTerrain : public WorldGenerator {
public:
	FastTerrain() {
		noise_.SetNoiseType(noise_.NoiseType_OpenSimplex2);
		noise_.SetFrequency(0.009f);
	}

	Chunk* Generate(const ChunkPos& pos) override;

	FastNoiseLite noise_;

	static constexpr double TREE_RAND_VAL = 0.5;
	static constexpr double TREE_RAND_VAL_RANGE = .01f;
};
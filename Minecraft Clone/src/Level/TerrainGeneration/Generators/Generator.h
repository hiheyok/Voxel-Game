#pragma once
#include "../../Chunk/Chunk.h"
#include "../../Chunk/TallChunk.h"
#include <glm/vec3.hpp>

class WorldGenerator {
public:
	WorldGenerator() {

	}

	static void SetSeed(long long worldSeedIn) {
		WorldGenerator::worldSeed = worldSeedIn;
	}

	virtual Chunk* Generate(const ChunkPos& pos) {
		return nullptr;
	}
	// TODO: Use custom tall chunk positioning
	virtual TallChunk* GenerateTall(const ChunkPos& pos) {
		return nullptr;
	}

	bool useTallChunks = false;

	static long long worldSeed;
};

_declspec(selectany) long long WorldGenerator::worldSeed = -1587754402;
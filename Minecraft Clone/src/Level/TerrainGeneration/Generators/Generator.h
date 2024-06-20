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

	virtual Chunk* Generate(glm::ivec3 Position) {
		return nullptr;
	}

	virtual TallChunk* GenerateTall(glm::ivec3 Position) {
		return nullptr;
	}

	bool useTallChunks = false;

	static long long worldSeed;
};

_declspec(selectany) long long WorldGenerator::worldSeed = -1587754402;
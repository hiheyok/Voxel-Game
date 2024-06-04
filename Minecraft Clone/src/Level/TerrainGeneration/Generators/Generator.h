#pragma once
#include "../../Chunk/Chunk.h"
#include <glm/vec3.hpp>

class WorldGenerator {
public:
	WorldGenerator() {

	}

	virtual Chunk* Generate(glm::ivec3 Position) {
		return nullptr;
	}

	const int SEED = 3453454;
};
#pragma once
#include "../../Chunk/Chunk.h"
#include <glm/vec3.hpp>

class WorldGenerator {
public:
	WorldGenerator() {

	}

	virtual Chunk* Generate(glm::ivec3 Position) {
		std::cout << "test\n";
		return nullptr;
	}

	const int SEED = 3453454;
};
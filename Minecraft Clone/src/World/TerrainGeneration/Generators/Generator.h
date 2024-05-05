#pragma once
#include "../../Chunk/Chunk.h"
#include <glm/vec3.hpp>

struct WorldGenerator {
	WorldGenerator() {

	}

	virtual Chunk* Generate(glm::ivec3 Position) {
		std::cout << "test\n";
	}

	const int SEED = 3453454;
};
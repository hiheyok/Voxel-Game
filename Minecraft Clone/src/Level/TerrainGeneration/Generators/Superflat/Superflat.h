#pragma once
#include "../Generator.h"

struct SuperflatWorld : WorldGenerator {
	SuperflatWorld() {

	}

	Chunk* Generate(glm::ivec3 Position) override;
};
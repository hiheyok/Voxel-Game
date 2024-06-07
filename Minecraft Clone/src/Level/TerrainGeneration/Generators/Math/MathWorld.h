#pragma once
#include "../Generator.h"

struct MathWorld : WorldGenerator {
	MathWorld() {

	}

	Chunk* Generate(glm::ivec3 Position) override;
};
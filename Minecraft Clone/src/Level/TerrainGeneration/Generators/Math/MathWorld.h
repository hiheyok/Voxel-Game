#pragma once
#include "../Generator.h"

class MathWorld : public WorldGenerator {
public:
	MathWorld() {

	}

	Chunk* Generate(glm::ivec3 Position) override;
};
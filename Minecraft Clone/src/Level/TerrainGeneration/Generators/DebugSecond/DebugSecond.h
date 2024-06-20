#pragma once
#include "../Generator.h"

class DebugWorldSecond : public WorldGenerator {
public:
	DebugWorldSecond() {

	}

	Chunk* Generate(glm::ivec3 Position) override;
};
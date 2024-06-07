#pragma once
#include "../Generator.h"

struct DebugWorldSecond : WorldGenerator {
	DebugWorldSecond() {

	}

	Chunk* Generate(glm::ivec3 Position) override;
};
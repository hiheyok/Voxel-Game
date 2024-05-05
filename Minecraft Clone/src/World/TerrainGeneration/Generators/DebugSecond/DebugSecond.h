#pragma once
#include "../Generator.h"

struct DebugWorldSecond : WorldGenerator {
	DebugWorldSecond() {

	}

	Chunk* Generate(glm::ivec3 Position) override {
		Chunk* chunk = new Chunk;

		chunk->SetBlock(Blocks.COBBLESTONE, 8, 8, 8);

		return chunk;
	}
};
#pragma once
#include "../Generator.h"

struct SuperflatWorld : WorldGenerator {
	SuperflatWorld() {

	}

	Chunk* Generate(glm::ivec3 Position) override {
		Chunk* chunk = new Chunk;
		int cy = Position.y * 16;

		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				for (int y = 0; y < 16; y++) {
					if (y + cy < 10) {
						chunk->SetBlockUnsafe(Blocks.WHITE_CONCRETE, x, y, z);
					}
				}
			}
		}
		return chunk;
	}
};
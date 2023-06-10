#pragma once

#include "ChunkData.h"

#include "../../Utils/FastNoiseLite.h"


class Chunk : public ChunkContainer {
public:
	
	void GenerateBlankChunk() {
		for (int x = 0; x < 16; x++) {
			for (int y = 0; y < 16; y++) {
				for (int z = 0; z < 16; z++) {
					Blocks.ChangeBlock(AIR, x, y, z);
				}
			}
		}
	}

	void Generate(FastNoiseLite* noise);
	

};
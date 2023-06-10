#pragma once

#include "ChunkData.h"

typedef unsigned long long int ChunkID;

class Chunk : public ChunkContainer {
public:
	
	void GenerateBlankChunk() {
		for (int x = 0; x < 16; x++) {
			for (int y = 0; y < 16; y++) {
				for (int z = 0; z < 16; z++) {
					Blocks.ChangeBlock(AIR.ID, x, y, z);
				}
			}
		}
	}

	

};
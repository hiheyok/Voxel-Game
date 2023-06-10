#pragma once

#include "DataContainer/ChunkMapData.h"

struct World {

	void Generate() {

		int r = 10;
		int size = 16;

		for (int x = -size; x <= size; x++) {
			for (int y = -size; y <= size; y++) {
				for (int z = -size; z <= size; z++) {
					
					GenerateChunk(x,y,z);

				}
			}
		}
	}

	void GenerateChunk(int x, int y, int z) {

		int offsetx = x * 16;
		int offsety = y * 16;
		int offsetz = z * 16;

		int radius = 10;

		for (int bx = 0; bx < 16; bx++) {
			for (int by = 0; by < 16; by++) {
				for (int bz = 0; bz < 16; bz++) {
					int gx = offsetx + bx;
					int gy = offsety + by;
					int gz = offsetz + bz;

					if (gx * gx + gy * gy + gz * gz < radius * radius) {
						if (!Chunks.Check(x,y,z)) {
							Chunk chunk;
							chunk.x = x;
							chunk.y = y;
							chunk.z = z;
							Chunks.InsertChunk(chunk, x, y, z);
						}

						Chunks.GetChunk(x, y, z).Blocks.ChangeBlock(STONE.ID, bx, by, bz);

						Chunks.GetChunk(x, y, z).isEmpty = false;
					}
				}
			}
		}
	}

	ChunkMap Chunks;
};
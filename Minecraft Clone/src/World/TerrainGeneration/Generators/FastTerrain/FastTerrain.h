#pragma once
#include "../Generator.h"

struct FastTerrain : WorldGenerator {
	FastTerrain() {
		Noise.SetNoiseType(Noise.NoiseType_OpenSimplex2);
		Noise.SetFrequency(0.009f);
	}

	Chunk* Generate(glm::ivec3 Position) override {
		Chunk* chunk = new Chunk;

		Position *= 16;

		int cx = Position.x;
		int cy = Position.y;
		int cz = Position.z;

		for (int x = 0 + cx; x < 16 + cx; x++) {
			for (int z = 0 + cz; z < 16 + cz; z++) {

				float BIOME_MAP = (float)(Noise.GetNoise((float)x / 600, (float)z / 600, (float)SEED) + 1) / 2;
				float TREE_MAP = (float)((double)(Noise.GetNoise((float)x * 10, (float)z * 10, (float)SEED) + 1) / 2) * (BIOME_MAP * 2);
				float MIN_BIOME_MAP = (float)((Noise.GetNoise((float)x / 60, (float)z / 60, (float)SEED) + 1)) * 2;
				float TEMP_MAP = (float)(Noise.GetNoise((float)x / 1000, (float)z / 1000, (float)SEED) + 1) * 2;
				float HEIGHT_MAP = (float)(((Noise.GetNoise((float)x, (float)z, (float)SEED) + 1) * 3) * (((Noise.GetNoise((float)x * 5, (float)z * 5, (float)SEED) + 1) * .25) * BIOME_MAP)) * 30;
				int a = (int)pow(((BIOME_MAP * HEIGHT_MAP + 5) * TEMP_MAP - 10) / (2 * MIN_BIOME_MAP), 1.6) + 5;
				for (int y = 0 + cy; y < 16 + cy; y++) {
					if ((a == y || a + 1 == y || a + 2 == y || a + 3 == y || a + 4 == y) && a > 10) {
						if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
							chunk->SetBlock(Blocks.OAK_LOG, x - cx, y - cy, z - cz);

						}
					}
					if (a + 5 == y && a > 10) {
						if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
							for (int tx = -2; tx <= 2; tx++) {
								for (int ty = -2; ty <= 2; ty++) {
									for (int tz = -2; tz <= 2; tz++) {
										chunk->SetBlock(Blocks.OAK_LEAF, x + tx - cx, y + ty - cy, z + tz - cz);
									}
								}
							}

						}
					}

					if (y < 10) {
						chunk->SetBlockUnsafe(Blocks.WATER, x - cx, y - cy, z - cz);
					}

					if (a > y) {
						if (y < 12) {
							chunk->SetBlockUnsafe(Blocks.SAND, x - cx, y - cy, z - cz);
						}
						else {

							if (a >= y) {
								chunk->SetBlockUnsafe(Blocks.GRASS, x - cx, y - cy, z - cz);
								chunk->isEmpty = false;
							}
							if (a - 1 > y) {
								chunk->SetBlockUnsafe(Blocks.DIRT, x - cx, y - cy, z - cz);
							}



						}

					}


				}

			}
		}

		return chunk;
	}

	FastNoiseLite Noise;

	double TREE_RAND_VAL = 0.5;
	double TREE_RAND_VAL_RANGE = .01f;
};
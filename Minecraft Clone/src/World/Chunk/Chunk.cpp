#include "Chunk.h"

constexpr auto SEED = 3453454;

double TREE_RAND_VAL = 0.5;
double TREE_RAND_VAL_RANGE = .01f;

void Chunk::Generate(FastNoiseLite* noise) {

	noise->SetFrequency(0.009f);

	int cx = Position.x * 16;
	int cz = Position.z * 16;
	int cy = Position.y * 16;


	for (int x = 0 + cx; x < 16 + cx; x++) {
		for (int z = 0 + cz; z < 16 + cz; z++) {

			float BIOME_MAP = (float)(noise->GetNoise((float)x / 600, (float)z / 600, (float)SEED) + 1) / 2;
			float TREE_MAP = (float)((double)(noise->GetNoise((float)x * 10, (float)z * 10, (float)SEED) + 1) / 2) * (BIOME_MAP * 2);
			float MIN_BIOME_MAP = (float)((noise->GetNoise((float)x / 60, (float)z / 60, (float)SEED) + 1)) * 2;
			float TEMP_MAP = (float)(noise->GetNoise((float)x / 1000, (float)z / 1000, (float)SEED) + 1) * 2;
			float HEIGHT_MAP = (float)(((noise->GetNoise((float)x, (float)z, (float)SEED) + 1) * 3) * (((noise->GetNoise((float)x * 5, (float)z * 5, (float)SEED) + 1) * .25) * BIOME_MAP)) * 30;
			int a = (int)pow(((BIOME_MAP * HEIGHT_MAP + 5) * TEMP_MAP - 10) / (2 * MIN_BIOME_MAP), 1.6) + 5;
			for (int y = 0 + cy; y < 16 + cy; y++) {
				if ((a == y || a + 1 == y || a + 2 == y || a + 3 == y || a + 4 == y) && a > 10) {
					if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
						Blocks.ChangeBlock(OAK_LOG,x - cx, y - cy, z - cz);
						isEmpty = false;

					}
				}
				/*if (a + 5 == y && a > 10) {
					if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
						for (int tx = -2; tx <= 2; tx++) {
							for (int ty = -2; ty <= 2; ty++) {
								for (int tz = -2; tz <= 2; tz++) {
									addblock(x + tx, y + ty, z + tz, OAK_LEAF);
								}
							}
						}

					}
				}*/

				/*if ((x == 16) || (z == 16) || (y == 16)) {
					Blocks.ChangeBlock(DIAMOND_BLOCK, x - cx, y - cy, z - cz);
				}*/

				if (y < 10) {
					Blocks.ChangeBlock(WATER, x - cx, y - cy, z - cz);
					isEmpty = false;
				}

				if (a > y) {
					if (y < 12) {
						Blocks.ChangeBlock(SAND, x - cx, y - cy, z - cz);
						isEmpty = false;
					}
					else {

						if (a >= y) {
							Blocks.ChangeBlock(GRASS, x - cx, y - cy, z - cz);
							isEmpty = false;
						}
						if (a - 1 > y) {
							Blocks.ChangeBlock(DIRT, x - cx, y - cy, z - cz);
							isEmpty = false;
						}

						//std::cout << a << "\n";



					}

				}


			}

		}
	}
	
}
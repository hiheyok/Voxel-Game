#include "Chunk.h"

#include <vector>
#include <map>
constexpr auto SEED = 3453454;

double TREE_RAND_VAL = 0.5;
double TREE_RAND_VAL_RANGE = .01f;

using namespace std;

vector<values2x> ContinentalnessInterpolation{
	values2x(0.f, 0.75f),
	values2x(0.2f, 0.82f),
	values2x(0.23f, 0.8f),
	values2x(0.28f, 0.5f),
	values2x(0.38f, 0.2f),
	values2x(0.75f, 0.1f),
	values2x(1.f, 0.3f),
};

vector<values2x> ErosionnessInterpolation{
	values2x(0.f, 0.75f),
	values2x(0.2f, 0.82f),
	values2x(0.23f, 0.8f),
	values2x(0.28f, 0.5f),
	values2x(0.38f, 0.2f),
	values2x(0.75f, 0.1f),
	values2x(1.f, 0.3f),
};

vector<values2x> PeaksValleyInterpolation{
	values2x(0.f, 0.0f),
	values2x(0.075f, 0.0f),
	values2x(0.15f, 0.25f),
	values2x(0.5f, 0.35f),
	values2x(0.6f, 0.75f),
	values2x(0.75f, 0.85f),
	values2x(0.9f, 0.83f),
	values2x(1.f, 0.84f),
};

void Chunk::GenSuperFlat() {
	int cy = Position.y * 16;

	float heightBias = 50;
	float noiseOffset = 0.3;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			for (int y = 0; y < 16; y++) {
				if (y + cy < 10) {
					SetBlockUnsafe(Blocks.WHITE_CONCRETE, x, y, z);
				}
			}
		}
	}
}


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
						SetBlock(Blocks.OAK_LOG, x - cx, y - cy, z - cz);

					}
				}
				if (a + 5 == y && a > 10) {
					if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
						for (int tx = -2; tx <= 2; tx++) {
							for (int ty = -2; ty <= 2; ty++) {
								for (int tz = -2; tz <= 2; tz++) {
									SetBlock(Blocks.OAK_LEAF, x + tx - cx, y + ty - cy, z + tz - cz);
								}
							}
						}

					}
				}

				if (y < 10) {
					SetBlockUnsafe(Blocks.WATER, x - cx, y - cy, z - cz);
				}

				if (a > y) {
					if (y < 12) {
						SetBlockUnsafe(Blocks.SAND, x - cx, y - cy, z - cz);
					}
					else {

						if (a >= y) {
							SetBlockUnsafe(Blocks.GRASS, x - cx, y - cy, z - cz);
							isEmpty = false;
						}
						if (a - 1 > y) {
							SetBlockUnsafe(Blocks.DIRT, x - cx, y - cy, z - cz);
						}



					}

				}


			}

		}
	}

}

void Chunk::GenerateBlankChunk() {
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			for (int z = 0; z < 16; z++) {
				SetBlock(Blocks.AIR, (unsigned int)x, (unsigned int)y, (unsigned int)z);
			}
		}
	}
}

void Chunk::GenerateV2(FastNoiseLite* noise) {

	noise->SetFrequency(0.005f);

	noise->SetNoiseType(noise->NoiseType_OpenSimplex2);

	//int cx = Position.x * 16;
	//int cz = Position.z * 16;
	int cy = Position.y * 16;

	float heightBias = 50;
	float noiseOffset = 0.3;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			float continental = continentialNoise(getNoise2D(x, z, 3, 0.3f, noise));
			float erosion = erosionNoise(getNoise2D(x + 4345, z + 6443, 3, 1.f, noise)) / 2.f;
			float pv = peaksandvalley(getNoise2D(x + 65345, z + 12323, 3, 4.f, noise)) / 8;

			for (int y = 0; y < 16; y++) {

				float gy = (float)(y + cy);

				float n = getNoise3D(x, y, z, 4, 1.f, noise);

				n = n + noiseOffset;

				n += continental;
				n += erosion;
				n += (pv / (heightBias * (n + 0.5))) * gy;

				n = n * exp(-gy / heightBias);

				if (n > 0.5f) {
					if (n < 0.54f) {
						SetBlockUnsafe(Blocks.GRASS, x, y, z);

						SetBlock(Blocks.DIRT, x, y - 1, z);


					}
					else {
						SetBlockUnsafe(Blocks.STONE, x, y, z);
					}


				}


			}
		}
	}

	GenerateEnvironment(noise);
	GenerateDecor(noise);
}

void Chunk::GenerateEnvironment(FastNoiseLite* noise) {

	//int cx = Position.x * 16;
	//int cz = Position.z * 16;
	int cy = Position.y * 16;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			for (int y = 0; y < 16; y++) {

				if (y + cy < 34) {
					if ((GetBlockUnsafe(x, y, z) == Blocks.AIR)) {
						SetBlockUnsafe(Blocks.WATER, x, y, z);
					}

					if ((GetBlockUnsafe(x, y, z) == Blocks.GRASS)) {
						SetBlockUnsafe(Blocks.SAND, x, y, z);
					}
				}
			}
		}
	}

}

void Chunk::GenerateDebug() {
	if (Position.y == 0) {
		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {
				SetBlockUnsafe(Blocks.WHITE_CONCRETE, x, 0, z);
			}

		}

		int i = 0;

		for (const auto& b : Blocks.BlockTypeData) {
			SetBlockUnsafe(b.first, (i & 0b1111), 1, i >> 4);
			i++;
		}
	}



}

void Chunk::GenerateDecor(FastNoiseLite* noise) {

	int cx = Position.x * 16;
	int cz = Position.z * 16;
	int cy = Position.y * 16;

	const int tree_height = 3;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			//Global Pos
			int gx = cx + x;
			int gz = cz + z;

			float TREE_MAP = (float)((double)(noise->GetNoise((float)gx * 100.f, (float)gz * 100.f, (float)SEED) + 1.f) / 2.f);
			for (int y = 0; y < 16; y++) {
				if (GetBlock(x, y - 1, z) == Blocks.GRASS) {

					if (TREE_MAP <= 0.04) {
						for (int tx = -2; tx <= 2; tx++) {
							for (int tz = -2; tz <= 2; tz++) {
								if ((abs(tx) == 2) && (abs(tz) == 2)) {
									continue;
								}

								for (int ty = tree_height; ty <= tree_height + 1; ty++) {
									SetBlock(Blocks.OAK_LEAF, x + tx, y + ty, z + tz);
								}

							}
						}

						for (int tx = -1; tx <= 1; tx++) {
							for (int tz = -1; tz <= 1; tz++) {


								for (int ty = tree_height + 2; ty <= tree_height + 3; ty++) {

									if ((abs(tx) == 1) && (abs(tz) == 1) && (ty == tree_height + 3)) {
										continue;
									}

									SetBlock(Blocks.OAK_LEAF, x + tx, y + ty, z + tz);
								}

							}
						}

						for (int ty = 0; ty < tree_height + 2; ty++) {
							SetBlock(Blocks.OAK_LOG, x, y + ty, z);
						}

					}




				}

			}
		}
	}


	int radius = 40;


	for (int x = 0 + cx; x < 16 + cx; x++) {
		for (int z = 0 + cz; z < 16 + cz; z++) {
			for (int y = 0 + cy; y < 16 + cy; y++) {
				if ((x * x) + (y - 140) * (y - 140) + z * z <= radius * radius) {
					SetBlock(Blocks.SAND, x - cx, y - cy, z - cz);
 
				}

				//if (y == 90) {
				//	SetBlock(Blocks.SAND, x - cx, y - cy, z - cz);
				//}
			}
		}
	}
}

void Chunk::UpdateGen() {
	for (int side = 0; side < 6; side++) {
		int axis = side >> 1;
		int face = side & 0b1;

		if (Neighbors[side] == nullptr)
			continue;

		if (Neighbors[side]->OutsideBlockToPlace[axis * 2 + (!face)].size() == 0) {
			continue;
		}

		std::vector<SetBlockRelative> blocks = Neighbors[side]->OutsideBlockToPlace[axis * 2 + (!face)];

		Neighbors[side]->OutsideBlockToPlace[axis * 2 + (!face)].clear();

		for (int i = 0; i < blocks.size(); i++) {
			SetBlock(blocks[i].m_block, blocks[i].m_x, blocks[i].m_y, blocks[i].m_z);
		}

		blocks.clear();
	}
}

float Chunk::getNoise3D(int x, int y, int z, int samples, float frequency, FastNoiseLite* noise) {
	int cx = Position.x * 16;
	int cz = Position.z * 16;
	int cy = Position.y * 16;

	float gx = (float)(x + cx);
	float gy = (float)(y + cy);
	float gz = (float)(z + cz);

	gx *= frequency;
	gy *= frequency;
	gz *= frequency;

	float out = 0.0f;

	for (int i = 0; i < samples; i++) {
		float n = (noise->GetNoise(gx * powf(2, i), gy * powf(2, i), gz * powf(2, i)) + 1) * 0.5f;
		out += n * powf(0.5f, i);
	}

	out = out * ((-0.5f) / (powf(0.5, samples) - 1));

	return out;

}
float Chunk::getNoise2D(int x, int z, int samples, float frequency, FastNoiseLite* noise) {
	int cx = Position.x * 16;
	int cz = Position.z * 16;

	float gx = (float)(x + cx);
	float gz = (float)(z + cz);

	gx *= frequency;
	gz *= frequency;

	float out = 0.0f;

	for (int i = 0; i < samples; i++) {
		float n = (noise->GetNoise(gx * powf(2, i), gz * powf(2, i)) + 1) * 0.5f;
		out += n * powf(0.5f, i);
	}

	out = out * ((-0.5f) / (powf(0.5, samples) - 1));

	return out;
}

int getIndex(vector<values2x>& vec, float bottomBound) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].x >= bottomBound) {
			return i - 1;
		}
	}

	return vec.size() - 1;
}

float Chunk::continentialNoise(float n) {
	int index = getIndex(ContinentalnessInterpolation, n);

	float x1 = ContinentalnessInterpolation[index].x;
	float y1 = ContinentalnessInterpolation[index].y;

	index++;

	float x2 = ContinentalnessInterpolation[index].x;
	float y2 = ContinentalnessInterpolation[index].y;

	float m = (y1 - y2) / (x1 - x2);

	float out = m * (n - x1) + y1;

	return out;

}

float Chunk::erosionNoise(float n) {
	int index = getIndex(ErosionnessInterpolation, n);

	float x1 = ErosionnessInterpolation[index].x;
	float y1 = ErosionnessInterpolation[index].y;

	index++;

	float x2 = ErosionnessInterpolation[index].x;
	float y2 = ErosionnessInterpolation[index].y;

	float m = (y1 - y2) / (x1 - x2);

	float out = m * (n - x1) + y1;

	return out;
}

float Chunk::peaksandvalley(float n) {
	int index = getIndex(PeaksValleyInterpolation, n);

	float x1 = PeaksValleyInterpolation[index].x;
	float y1 = PeaksValleyInterpolation[index].y;

	index++;

	float x2 = PeaksValleyInterpolation[index].x;
	float y2 = PeaksValleyInterpolation[index].y;

	float m = (y1 - y2) / (x1 - x2);

	float out = m * (n - x1) + y1;

	return out;
}
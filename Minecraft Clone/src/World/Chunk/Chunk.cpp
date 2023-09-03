#include "Chunk.h"

#include <vector>
#include <map>
constexpr auto SEED = 3453454;

double TREE_RAND_VAL = 0.5;
double TREE_RAND_VAL_RANGE = .01f;

using namespace std;

map<float, float> Continentalness = {
	{0.f, 0.75f},
	{0.2f, 0.82f},
	{0.23f, 0.8f},
	{0.28f, 0.5f},
	{0.38f, 0.2f},
	{0.75f, 0.1f},
	{1.f, 0.3f},
};

map<float, float> Erosionness = {
	{0.f, 0.9f},
	{0.075f, 0.5f},
	{0.15f, 0.55f},
	{0.3f, 0.3f},
	{0.75f, 0.1f},
	{0.8f, 0.3f},
	{0.9f, 0.2f},
	{1.f, 0.0f},
};

map<float, float> PeaksValley = {
	{0.f, 0.0f},
	{0.075f, 0.0f},
	{0.15f, 0.25f},
	{0.5f, 0.35f},
	{0.6f, 0.75f},
	{0.75f, 0.85f},
	{0.9f, 0.83f},
	{1.f, 0.84f},
};



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
						SetBlock(OAK_LOG,x - cx, y - cy, z - cz);
						isEmpty = false;

					}
				}
				if (a + 5 == y && a > 10) {
					if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
						for (int tx = -2; tx <= 2; tx++) {
							for (int ty = -2; ty <= 2; ty++) {
								for (int tz = -2; tz <= 2; tz++) {
									SetBlock(OAK_LEAF,x + tx - cx, y + ty - cy, z + tz - cz);
								}
							}
						}

					}
				}

				/*if ((x == 16) || (z == 16) || (y == 16)) {
					SetBlock(DIAMOND_BLOCK, x - cx, y - cy, z - cz);
				}*/

				if (y < 10) {
					SetBlock(WATER, x - cx, y - cy, z - cz);
					isEmpty = false;
				}

				if (a > y) {
					if (y < 12) {
						SetBlock(SAND, x - cx, y - cy, z - cz);
						isEmpty = false;
					}
					else {

						if (a >= y) {
							SetBlock(GRASS, x - cx, y - cy, z - cz);
							isEmpty = false;
						}
						if (a - 1 > y) {
							SetBlock(DIRT, x - cx, y - cy, z - cz);
							isEmpty = false;
						}

						//std::cout << a << "\n";



					}

				}


			}

		}
	}
	
}

void Chunk::GenerateV2(FastNoiseLite* noise) {

	noise->SetFrequency(0.005f);

	noise->SetNoiseType(noise->NoiseType_OpenSimplex2);

	int cx = Position.x * 16;
	int cz = Position.z * 16;
	int cy = Position.y * 16;

	float heightBias = 50;
	float noiseOffset = 0.3;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			float continental = continentialNoise(getNoise2D(x, z, 3,0.3f, noise));
			float erosion = erosionNoise(getNoise2D(x + 4345, z + 6443, 3, 1.f, noise)) / 2.f;
			float pv = peaksandvalley(getNoise2D(x + 65345, z + 12323, 3, 4.f, noise)) / 8;

			for (int y = 0; y < 16; y++) {

				float gy = (float)(y + cy);

				float n =  getNoise3D(x, y, z, 4,1.f, noise);

				n = n + noiseOffset;

				n += continental;
				n += erosion;
				n += (pv / (heightBias * (n + 0.5))) * gy;

				n = n * exp(-gy / heightBias);

				//getLogger()->LogInfo("Chunk Generator", "Continential Noise: " + to_string(continental));
				if (n > 0.5f) {
					if (n < 0.54f) {
						SetBlock(DIRT, x, y, z);
						SetBlock(GRASS, x, y + 1, z);
					}
					else {
						SetBlock(STONE, x, y, z);
					}

					
				}


			}
		}
	}

	GenerateEnvironment(noise);
	GenerateDecor(noise);
}

void Chunk::GenerateEnvironment(FastNoiseLite* noise) {
	
	int cx = Position.x * 16;
	int cz = Position.z * 16;
	int cy = Position.y * 16;
	
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			for (int y = 0; y < 16; y++) {
				if (y + cy < 35) {
					if ((GetBlock(x, y, z) != DIRT) && (GetBlock(x, y, z) != STONE)) {
						SetBlock(WATER, x, y, z);
					}
				}
				


			}
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
				if (GetBlock(x, y - 1, z) == GRASS) {

					if (TREE_MAP <= 0.04) {
						for (int tx = -2; tx <= 2; tx++) {
							for (int tz = -2; tz <= 2; tz++) {
								if ((abs(tx) == 2) && (abs(tz) == 2)) {
									continue;
								}

								for (int ty = tree_height; ty <= tree_height + 1; ty++) {
									SetBlock(OAK_LEAF, x + tx, y + ty, z + tz);
								}

							}
						}

						for (int tx = -1; tx <= 1; tx++) {
							for (int tz = -1; tz <= 1; tz++) {
								

								for (int ty = tree_height + 2; ty <= tree_height + 3; ty++) {

									if ((abs(tx) == 1) && (abs(tz) == 1) && (ty == tree_height + 3)) {
										continue;
									}

									SetBlock(OAK_LEAF, x + tx, y + ty, z + tz);
								}

							}
						}

						for (int ty = 0; ty < tree_height + 2; ty++) {
							SetBlock(OAK_LOG, x, y + ty, z);
						}

					}

					


				}

			}
		}
	}
}

void Chunk::UpdateGen() {
	for (int axis = 0; axis < 3; axis++) {
		for (int face = 0; face < 2; face++) {
			int index = axis * 2 + face;

			if (Neighbors[index] == nullptr) {
				continue;
			}

			std::vector<ChunkBlockPlace::SetBlockRelative> blocks = Neighbors[index]->OutsideBlockToPlace[axis * 2 + (!face)];

			//if (blocks.size() != 0) {
			//	getLogger()->LogInfo("World Gen", std::to_string(blocks.size()));
			//}
			
			

			Neighbors[index]->OutsideBlockToPlace[axis * 2 + (!face)].clear();

			for (const auto& b : blocks) {
				SetBlock(b.m_block, b.m_x, b.m_y, b.m_z);
			}

			blocks.clear();
		}
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

map<float, float>::iterator getItr(std::map<float, float>& map, float bottomBound) {
	std::map<float, float>::iterator itr = map.begin();
	
	while (itr != map.end()) {
		if (itr->first >= bottomBound) {
			return --itr;
		}
		itr++;
	}
	return itr;
}

float Chunk::continentialNoise(float n) {
	std::map<float, float>::iterator itr = getItr(Continentalness, n);

	float x1 = itr->first;
	float y1 = itr->second;

	itr++;

	float x2 = itr->first;
	float y2 = itr->second;

	float m = (y1 - y2) / (x1 - x2);

	float out = m * (n - x1) + y1;

	return out;

}

float Chunk::erosionNoise(float n) {
	std::map<float, float>::iterator itr = getItr(Continentalness, n);

	float x1 = itr->first;
	float y1 = itr->second;

	itr++;

	float x2 = itr->first;
	float y2 = itr->second;

	float m = (y1 - y2) / (x1 - x2);

	float out = m * (n - x1) + y1;

	return out;
}

float Chunk::peaksandvalley(float n) {
	std::map<float, float>::iterator itr = getItr(PeaksValley, n);

	float x1 = itr->first;
	float y1 = itr->second;

	itr++;

	float x2 = itr->first;
	float y2 = itr->second;

	float m = (y1 - y2) / (x1 - x2);

	float out = m * (n - x1) + y1;

	return out;
}
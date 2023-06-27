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
			float erosion = continentialNoise(getNoise2D(x + 4345, z + 6443, 3, 0.3f, noise));

			for (int y = 0; y < 16; y++) {

				float gx = (float)(x + cx);
				float gy = (float)(y + cy);
				float gz = (float)(z + cz);

				float n =  getNoise3D(x, y, z, 4,1.f, noise);

				//getLogger()->LogInfo("Generator" , "Noise: " + std::to_string(n));

				n = n + noiseOffset;

				n += continental;
				n += erosion;

				n = n * exp(-gy / heightBias);

				//getLogger()->LogInfo("Chunk Generator", "Continential Noise: " + to_string(continental));
				if (n > 0.5f) {
					if (n < 0.54f) {
						Blocks.ChangeBlock(DIRT, (uint32_t)x, (uint32_t)y, (uint32_t)z);
						isEmpty = false;
					}
					else {
						Blocks.ChangeBlock(STONE, (uint32_t)x, (uint32_t)y, (uint32_t)z);
						isEmpty = false;
					}

					
				}


			}
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
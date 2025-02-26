#include "Mountains.h"

Chunk* MountainGenerator::Generate(const ChunkPos& pos) {
	Chunk* chunk = new Chunk;

	ChunkPos scaledPos = pos;
	scaledPos *= 16;

	float heightBias = 50;
	float noiseOffset = 0.3;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			float continental = continentialNoise(getNoise2D(glm::ivec2(pos.x, pos.z), glm::ivec2(x, z), 3, 0.3f));
			float erosion = erosionNoise(getNoise2D(glm::ivec2(pos.x, pos.z), glm::ivec2(x + 4345, z + 6443), 3, 1.f)) / 2.f;
			float pv = peaksandvalley(getNoise2D(glm::ivec2(pos.x, pos.z), glm::ivec2(x + 65345, z + 12323), 3, 4.f)) / 8;

			for (int y = 0; y < 16; y++) {

				float gy = (float)(y + scaledPos.y);

				float n = getNoise3D(glm::ivec3{ pos.x,pos.y ,pos.z }, glm::ivec3(x, y, z), 4, 1.f);

				n = n + noiseOffset;

				n += continental;
				n += erosion;
				n += (pv / (heightBias * (n + 0.5))) * gy;

				n = n * exp(-gy / heightBias);

				if (n > 0.5f) {
					if (n < 0.54f) {
						chunk->SetBlockUnsafe(Blocks.GRASS, x, y, z);
						chunk->SetBlock(Blocks.DIRT, x, y - 1, z);
					}
					else {
						chunk->SetBlockUnsafe(Blocks.STONE, x, y, z);
					}
				}
			}
		}
	}

	GenerateEnvironment(scaledPos, chunk);
	GenerateDecor(scaledPos, chunk);

	int gx = scaledPos.x;
	int gz = scaledPos.y;
	int gy = scaledPos.z;


	if (pos.y == 3) {
		int numBlocks = static_cast<int>(Blocks.BlockTypeData.size());

		for (int x = 0; x < 16; x++) {
			for (int z = 0; z < 16; z++) {

				int px = x + gx;
				int pz = z + gz;

				if (((px & 0b1) == 1) || ((pz & 0b1) == 1)) {
					continue;
				}

				px = px / 2;
				pz = pz / 2;

				if ((px < 0) || (px >= 20)) {
					continue;
				}

				int b = px + pz * 20;

				if ((b < numBlocks) && (b >= 0)) {
					chunk->SetBlockUnsafe(b, x, 3, z);
				}
			}
		}
	}




	return chunk;
}

void MountainGenerator::GenerateEnvironment(const ChunkPos& pos, Chunk* chunk) {

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			for (int y = 0; y < 16; y++) {

				if (y + pos.y < 34) {
					if ((chunk->GetBlockUnsafe(x, y, z) == Blocks.AIR)) {
						chunk->SetBlockUnsafe(Blocks.BLUE_CONCRETE, x, y, z);
					}

					if ((chunk->GetBlockUnsafe(x, y, z) == Blocks.GRASS)) {
						chunk->SetBlockUnsafe(Blocks.SAND, x, y, z);
					}
				}
			}
		}
	}
}

void MountainGenerator::GenerateDecor(const ChunkPos& pos, Chunk* chunk) {
	const int tree_height = 3;

	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			//Global Pos
			int gx = pos.x + x;
			int gz = pos.z + z;

			float TREE_MAP = (float)((double)(Noise.GetNoise((float)gx * 100.f, (float)gz * 100.f, 3453454.f) + 1.f) / 2.f);
			for (int y = 0; y < 16; y++) {
				if (chunk->GetBlock(x, y - 1, z) == Blocks.GRASS) {

					if (TREE_MAP <= 0.04) {
						
						for (int tx = -2; tx <= 2; tx++) {
							for (int tz = -2; tz <= 2; tz++) {
								if ((abs(tx) == 2) && (abs(tz) == 2))
									continue;
								
								for (int ty = tree_height; ty <= tree_height + 1; ty++)
									chunk->SetBlock(Blocks.OAK_LEAF, x + tx, y + ty, z + tz);

							}
						}

						for (int tx = -1; tx <= 1; tx++) {
							for (int tz = -1; tz <= 1; tz++) {
								for (int ty = tree_height + 2; ty <= tree_height + 3; ty++) {

									if ((abs(tx) == 1) && (abs(tz) == 1) && (ty == tree_height + 3)) {
										continue;
									}
									chunk->SetBlock(Blocks.OAK_LEAF, x + tx, y + ty, z + tz);
								}
							}
						}
						for (int ty = 0; ty < tree_height + 2; ty++) {
							chunk->SetBlock(Blocks.OAK_LOG, x, y + ty, z);
						}
					}
				}
			}
		}
	}

	//Cube 

	int xSize = 316;
	int ySize = 100;
	int zSize = 316;

	int xOffset = 0;
	int yOffset = 160;
	int zOffset = 0;

	/*for (int x = 0 + Position.x; x < 16 + Position.x; x++) {
		for (int z = 0 + Position.z; z < 16 + Position.z; z++) {
			for (int y = 0 + Position.y; y < 16 + Position.y; y++) {
				if (((x - xOffset) >= 0) && ((x - xOffset) < xSize) &&
					((y - yOffset) >= 0) && ((y - yOffset) < ySize) &&
					((z - zOffset) >= 0) && ((z - zOffset) < zSize)) {
					chunk->SetBlock(Blocks.SAND, x - Position.x, y - Position.y, z - Position.z);
				}
			}
		}
	}*/


	int radius = 40;

	for (int x = 0 + pos.x; x < 16 + pos.x; x++) {
		for (int z = 0 + pos.z; z < 16 + pos.z; z++) {
			for (int y = 0 + pos.y; y < 16 + pos.y; y++) {
				if ((x * x) + (y - 140) * (y - 140) + z * z <= radius * radius) {
					chunk->SetBlock(Blocks.SAND, x - pos.x, y - pos.y, z - pos.z);
				}

				//if (y == 90) {
				//	SetBlock(Blocks.SAND, x - cx, y - cy, z - cz);
				//}
			}
		}
	}
}


float MountainGenerator::getNoise3D(glm::ivec3 ChunkCoordinate, glm::ivec3 RelativeBlockCoords, int samples, float frequency) {
	glm::vec3 GlobalBlockPosition = glm::vec3(ChunkCoordinate * 16 + RelativeBlockCoords);

	GlobalBlockPosition *= frequency;

	float out = 0.0f;

	for (int i = 0; i < samples; i++) {
		float n = (Noise.GetNoise(GlobalBlockPosition.x * powf(2, i), GlobalBlockPosition.y * powf(2, i), GlobalBlockPosition.z * powf(2, i)) + 1) * 0.5f;
		out += n * powf(0.5f, i);
	}

	out = out * ((-0.5f) / (powf(0.5, samples) - 1));

	return out;

}
float MountainGenerator::getNoise2D(glm::ivec2 ChunkCoordinate, glm::ivec2 RelativeBlockCoords, int samples, float frequency) {
	glm::vec2 GlobalBlockPosition = glm::vec2(ChunkCoordinate * 16 + RelativeBlockCoords);

	GlobalBlockPosition *= frequency;

	float out = 0.0f;

	for (int i = 0; i < samples; i++) {
		float n = (Noise.GetNoise(GlobalBlockPosition.x * powf(2, i), GlobalBlockPosition.y * powf(2, i)) + 1) * 0.5f;
		out += n * powf(0.5f, i);
	}

	out = out * ((-0.5f) / (powf(0.5, samples) - 1));

	return out;
}


float MountainGenerator::continentialNoise(float n) {
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

float MountainGenerator::erosionNoise(float n) {
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

float MountainGenerator::peaksandvalley(float n) {
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

int MountainGenerator::getIndex(std::vector<glm::vec2>& vec, float bottomBound) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].x >= bottomBound) {
			return i - 1;
		}
	}

	return vec.size() - 1;
}
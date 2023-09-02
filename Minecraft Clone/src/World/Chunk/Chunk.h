#pragma once

#include "ChunkData.h"



#include "../../Utils/FastNoiseLite.h"


class Chunk : public ChunkContainer {
public:
	
	void GenerateBlankChunk() {
		for (int x = 0; x < 16; x++) {
			for (int y = 0; y < 16; y++) {
				for (int z = 0; z < 16; z++) {
					SetBlock(AIR, (unsigned int)x, (unsigned int)y, (unsigned int)z);
				}
			}
		}
	}

	void Generate(FastNoiseLite* noise);
	
	void GenerateV2(FastNoiseLite* noise);

private:

	float getNoise3D(int x, int y, int z, int samples, float frequency, FastNoiseLite* noise);
	float getNoise2D(int x, int z, int samples,float frequency, FastNoiseLite* noise);

	float continentialNoise(float n);
	float erosionNoise(float n);
	float peaksandvalley(float n);
};
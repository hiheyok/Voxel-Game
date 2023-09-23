#pragma once

#include "ChunkData.h"
#include "../../Utils/FastNoiseLite.h"

struct values2x {
	float x = 0;
	float y = 0;

	values2x(float x_, float y_) : x(x_), y(y_) {}
};

enum TerrainType {
	SUPERFLAT, FASTTERRAIN, MOUNTAINS
};


class Chunk : public ChunkContainer {
public:
	
	Chunk(int x, int y, int z, TerrainType type, FastNoiseLite& noise) {
		SetPosition(x, y, z);

		switch (type) {
		case SUPERFLAT:
			GenSuperFlat();
			break;
		case FASTTERRAIN:
			Generate(&noise);
			break;
		case MOUNTAINS:
			GenerateV2(&noise);
			break;
		}
	}

	Chunk() {

	}

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

	void GenSuperFlat();

	void UpdateGen();

private:

	//Generate Decor
	void GenerateDecor(FastNoiseLite* noise);

	//Gen env
	void GenerateEnvironment(FastNoiseLite* noise);

	float getNoise3D(int x, int y, int z, int samples, float frequency, FastNoiseLite* noise);
	float getNoise2D(int x, int z, int samples,float frequency, FastNoiseLite* noise);

	float continentialNoise(float n);
	float erosionNoise(float n);
	float peaksandvalley(float n);
public:
	bool operator==(const Chunk& other) const = default;
};
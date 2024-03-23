#pragma once

#include "ChunkData.h"
#include "../../Utils/FastNoiseLite.h"

struct values2x {
	float x = 0;
	float y = 0;

	values2x(float x_, float y_) : x(x_), y(y_) {}
};

enum TerrainType {
	SUPERFLAT, FASTTERRAIN, MOUNTAINS, DEBUG_WORLD, MATH_WORLD
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
		case DEBUG_WORLD:
			GenerateDebug();
			break;
		case MATH_WORLD:
			GenerateMath();
			break;
		}
	}

	Chunk() {

	}

	void GenerateBlankChunk();

	void GenerateDebug();

	void Generate(FastNoiseLite* noise);
	
	void GenerateV2(FastNoiseLite* noise);

	void GenSuperFlat();

	void UpdateGen();

private:

	void GenerateMath();

	//Generate Decor
	void GenerateDecor(FastNoiseLite* noise);

	//Gen env
	void GenerateEnvironment(FastNoiseLite* noise);

	float getNoise3D(int x, int y, int z, int samples, float frequency, FastNoiseLite* noise);
	float getNoise2D(int x, int z, int samples,float frequency, FastNoiseLite* noise);

	float continentialNoise(float n);
	float erosionNoise(float n);
	float peaksandvalley(float n);
};
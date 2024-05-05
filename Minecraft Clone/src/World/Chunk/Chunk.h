#pragma once

#include "ChunkData.h"
#include "../../Utils/FastNoiseLite.h"

struct values2x {
	float x = 0;
	float y = 0;

	values2x(float x_, float y_) : x(x_), y(y_) {}
};

enum TerrainType {
	SUPERFLAT, FASTTERRAIN, MOUNTAINS, DEBUG_WORLD, MATH_WORLD, DEBUG_WORLD_2
};


class Chunk : public ChunkContainer {
public:

	Chunk() {

	}

	void UpdateGen();

private:

};
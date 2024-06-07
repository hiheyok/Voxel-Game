#pragma once
#include "../Generator.h"
#include "../../Noisemaps/Noisemap.h"
#include "../../Noisemaps/TerrainParameters.h"
#include "Overworld.h"


class MinecraftTerrain : public WorldGenerator {
public:
	MinecraftTerrain() {
		
	}

	Chunk* Generate(glm::ivec3 Position) override;

private:
	NoiseMaps1122 Noisemaps;
	TerrainParamters Paramters;

	double* heightMap;

	glm::vec3 VecFloor(glm::vec3 v);
};
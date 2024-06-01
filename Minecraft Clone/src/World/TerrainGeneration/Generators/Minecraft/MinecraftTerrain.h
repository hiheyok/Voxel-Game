#pragma once
#include "../Generator.h"
#include "../../Noisemaps/Noisemap.h"
#include "../../Noisemaps/TerrainParameters.h"
#include "../../Biome/BiomeMapping.h"
#include "Overworld.h"


class MinecraftTerrain : public WorldGenerator {
public:
	MinecraftTerrain() {
		
	}

	Chunk* Generate(glm::ivec3 Position) override {
		Chunk* chunk = new Chunk;
		chunk->Position = Position;

		ChunkGeneratorSettings settings;

		OverworldGenerator gen = OverworldGenerator(SEED, settings);
		gen.setBlocksInChunk(Position.x, Position.z, chunk);

		return chunk;
	}
private:
	NoiseMaps1122 Noisemaps;
	TerrainParamters Paramters;
	BiomeMapping BiomeMap;

	double* heightMap;

	glm::vec3 VecFloor(glm::vec3 v) {
		return glm::vec3(floor(v.x), floor(v.y), floor(v.z));
	}



};
#pragma once
#include "../Generator.h"
#include "../../Noisemaps/Noisemap.h"
#include "../../Noisemaps/TerrainParameters.h"
#include "Overworld.h"
#include "../../../Chunk/Heightmap/Heightmap.h"
#include "../../Biome/BiomeProvider.h"

class MinecraftTerrain : public WorldGenerator {
public:
	MinecraftTerrain() {
		settings_ = new ChunkGeneratorSettings;
		use_tall_chunks_ = true;
		
	}

	TallChunk* GenerateTall(const ChunkPos& pos) override;

private:
	void Init();

	NoiseMaps1122 Noisemaps;
	TerrainParamters Paramters;
	ChunkGeneratorSettings* settings_ = nullptr;
	

	//double* heightMap;

	//bool isInitialized = false;

	glm::vec3 VecFloor(glm::vec3 v);
};
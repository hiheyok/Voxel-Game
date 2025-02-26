#include "MinecraftTerrain.h"
#include "../../Biome/Biomes.h"
#include "../../Biome/BiomeProvider.h"
#include "../../GenLayer/CombineLayers.h"
using namespace glm;

vec3 MinecraftTerrain::VecFloor(vec3 v) {
	return vec3(floor(v.x), floor(v.y), floor(v.z));
}

TallChunk* MinecraftTerrain::GenerateTall(const ChunkPos& pos) {
	OverworldGenerator overworldGenerator = OverworldGenerator(WorldGenerator::worldSeed, *settings);

	TallChunk* chunk = new TallChunk();
	chunk->SetPosition(pos.x, pos.y, pos.z);
	
	overworldGenerator.GenerateChunk(pos.x, pos.z, chunk);

	return chunk;
}

void MinecraftTerrain::Init() {
	
	return;
}
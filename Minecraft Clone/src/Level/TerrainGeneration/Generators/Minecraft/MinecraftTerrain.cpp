#include "MinecraftTerrain.h"
#include "../../Biome/Biomes.h"
#include "../../Biome/BiomeProvider.h"
#include "../../GenLayer/CombineLayers.h"
using namespace glm;

vec3 MinecraftTerrain::VecFloor(vec3 v) {
	return vec3(floor(v.x), floor(v.y), floor(v.z));
}

TallChunk* MinecraftTerrain::GenerateTall(ivec3 Position) {
	OverworldGenerator overworldGenerator = OverworldGenerator(WorldGenerator::worldSeed, *settings);

	TallChunk* chunk = new TallChunk();
	chunk->SetPosition(Position.x, Position.y, Position.z);
	
	overworldGenerator.GenerateChunk(Position.x, Position.z, chunk);

	return chunk;
}

void MinecraftTerrain::Init() {
	
	return;
}
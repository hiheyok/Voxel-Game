#include "MinecraftTerrain.h"

using namespace glm;

vec3 MinecraftTerrain::VecFloor(vec3 v) {
	return vec3(floor(v.x), floor(v.y), floor(v.z));
}

Chunk* MinecraftTerrain::Generate(ivec3 Position) {
	Chunk* chunk = new Chunk;
	chunk->Position = Position;

	ChunkGeneratorSettings settings;

	OverworldGenerator gen = OverworldGenerator(SEED, settings);
	gen.setBlocksInChunk(Position.x, Position.z, chunk);

	return chunk;
}
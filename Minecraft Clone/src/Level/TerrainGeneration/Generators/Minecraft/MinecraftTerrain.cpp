#include "MinecraftTerrain.h"
#include "../../Biome/Biomes.h"
#include "../../Biome/BiomeProvider.h"
#include "../../GenLayer/CombineLayers.h"
using namespace glm;

vec3 MinecraftTerrain::VecFloor(vec3 v) {
    return vec3(floor(v.x), floor(v.y), floor(v.z));
}

void MinecraftTerrain::GenerateTall(const ChunkPos& pos, std::unique_ptr<TallChunk>& chunk) {
    OverworldGenerator overworldGenerator = OverworldGenerator(WorldGenerator::world_seed_, *settings_);
    chunk->SetPosition(pos.x, pos.y, pos.z);
    
    overworldGenerator.GenerateChunk(pos.x, pos.z, chunk.get());
}

void MinecraftTerrain::Init() {
    
    return;
}
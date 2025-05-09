#include "Level/TerrainGeneration/Generators/Minecraft/MinecraftTerrain.h"

#include "Level/TerrainGeneration/Biome/BiomeProvider.h"
#include "Level/TerrainGeneration/Generators/Minecraft/Overworld.h"
#include "Level/TerrainGeneration/Noisemaps/Noisemap.h"
#include "Level/TerrainGeneration/Noisemaps/TerrainParameters.h"

MinecraftTerrain::MinecraftTerrain()
    : noisemaps_{std::make_unique<NoiseMaps1122>()},
      parameters_{std::make_unique<TerrainParamters>()},
      settings_{std::make_unique<ChunkGeneratorSettings>()} {
  use_tall_chunks_ = true;
}

glm::vec3 MinecraftTerrain::VecFloor(glm::vec3 v) {
  return glm::vec3(floor(v.x), floor(v.y), floor(v.z));
}

void MinecraftTerrain::GenerateTall(ChunkPos pos,
                                    std::unique_ptr<TallChunk>& chunk) {
  OverworldGenerator overworldGenerator =
      OverworldGenerator(WorldGenerator::world_seed_, *settings_);
  chunk->SetPosition(pos.x, pos.y, pos.z);

  overworldGenerator.GenerateChunk(pos.x, pos.z, chunk.get());
}

void MinecraftTerrain::Init() { return; }

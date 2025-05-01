#pragma once
#include <glm/vec3.hpp>
#include <memory>

#include "Level/TerrainGeneration/Generators/Generator.h"

class NoiseMaps1122;

struct TerrainParamters;
struct ChunkGeneratorSettings;

class MinecraftTerrain : public WorldGenerator {
 public:
  MinecraftTerrain();

  void GenerateTall(ChunkPos pos, std::unique_ptr<TallChunk>& chunk) override;

 private:
  void Init();

  std::unique_ptr<NoiseMaps1122> noisemaps_;
  std::unique_ptr<TerrainParamters> parameters_;
  std::unique_ptr<ChunkGeneratorSettings> settings_;

  // double* heightMap;

  // bool isInitialized = false;

  glm::vec3 VecFloor(glm::vec3 v);
};

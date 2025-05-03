#pragma once
#include <memory>
#include <glm/vec3.hpp>

#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/TallChunk.h"

class WorldGenerator {
 public:
  WorldGenerator() {}

  static void SetSeed(int64_t worldSeedIn) {
    WorldGenerator::world_seed_ = worldSeedIn;
  }

  virtual void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
    (void)pos;
    (void)chunk;
    g_logger.LogError("WorldGenerator::Generate", "Don't use");
  }
  virtual void GenerateTall(ChunkPos pos, std::unique_ptr<TallChunk>& chunk) {
    (void)pos;
    (void)chunk;
    g_logger.LogError("WorldGenerator::GenerateTall", "Don't use");
  }

  bool use_tall_chunks_ = false;

  static int64_t world_seed_;
};

inline int64_t WorldGenerator::world_seed_ = -1587754402;

#pragma once
#include <glm/vec3.hpp>
#include <memory>
#include <stdexcept>

#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/TallChunk.h"

class GameContext;

class WorldGenerator {
 public:
  WorldGenerator(GameContext&);
  virtual ~WorldGenerator();

  static void SetSeed(int64_t worldSeedIn);

  virtual void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk);
  virtual void GenerateTall(ChunkPos pos, std::unique_ptr<TallChunk>& chunk);

  GameContext& game_context_;
  bool use_tall_chunks_ = false;

  static int64_t world_seed_;
};

inline int64_t WorldGenerator::world_seed_ = -1587754402;

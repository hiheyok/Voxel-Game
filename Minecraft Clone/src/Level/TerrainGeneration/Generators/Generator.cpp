#include "Level/TerrainGeneration/Generators/Generator.h"

#include <stdexcept>

WorldGenerator::WorldGenerator(GameContext& game_context)
    : game_context_{game_context} {}

WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::SetSeed(int64_t worldSeedIn) {
  WorldGenerator::world_seed_ = worldSeedIn;
}

void WorldGenerator::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  (void)pos;
  (void)chunk;
  throw std::runtime_error(
      "WorldGenerator::Generate - Function hasn't been implemented yet.");
}
void WorldGenerator::GenerateTall(ChunkPos pos,
                                  std::unique_ptr<TallChunk>& chunk) {
  (void)pos;
  (void)chunk;
  throw std::runtime_error(
      "WorldGenerator::GenerateTall - Function hasn't been implemented yet.");
}
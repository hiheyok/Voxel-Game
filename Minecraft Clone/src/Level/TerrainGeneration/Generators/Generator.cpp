#include "Level/TerrainGeneration/Generators/Generator.h"

#include <cstdint>
#include <memory>
#include <stdexcept>

#include "Core/Position/PositionTypes.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/TallChunk.h"

WorldGenerator::WorldGenerator(GameContext& context) : context_{context} {}

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
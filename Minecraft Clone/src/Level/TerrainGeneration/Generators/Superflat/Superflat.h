#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class SuperflatWorld : public WorldGenerator {
 public:
  explicit SuperflatWorld(GameContext&);
  ~SuperflatWorld();

  void Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) override;
};

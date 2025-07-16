#pragma once

#include "Level/Light/LightEngine.h"

class BlockLightEngine : public LightEngine {
 public:
  BlockLightEngine(GameContext&, WorldInterface&);
  ~BlockLightEngine() override;

  void LightChunk(ChunkPos chunk_pos) override;
  void CheckNeighborChunk(ChunkPos center_chunk) override;
  void CheckBlock(BlockPos block_pos) override;
  void PropagateChanges(const ChunkLightTask& tasks) override;
};

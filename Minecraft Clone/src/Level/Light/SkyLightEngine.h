#pragma once

#include <array>

#include "Core/Typenames.h"
#include "Level/Light/LightEngine.h"


class SkyLightEngine : public LightEngine {
 public:
  SkyLightEngine(GameContext&, WorldInterface&);
  ~SkyLightEngine() override;

  void LightChunk(ChunkPos chunk_pos) override;
  void CheckNeighborChunk(ChunkPos center_chunk) override;
  void CheckBlock(BlockPos block_pos) override;
  void PropagateChanges(const ChunkLightTask& tasks) override;

 protected:

 private:
  void DelayDecrease();
  void DelayIncrease();

  // Returns bottom when it hit an opaque block
  int TryPropagateSkylight(BlockPos block_pos);
  // Propagate shadow until it hits a block with no light
  void TryPropagateShadow(BlockPos block_pos);

  std::array<int8_t, kChunkSize2D> heightmap_block_change_;
};

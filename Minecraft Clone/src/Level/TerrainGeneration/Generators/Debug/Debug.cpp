#include "Level/TerrainGeneration/Generators/Debug/Debug.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"

DebugWorld::DebugWorld(GameContext& context) : WorldGenerator{context} {}
DebugWorld::~DebugWorld() = default;

void DebugWorld::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  int gx = pos.x * kChunkDim;
  int gz = pos.z * kChunkDim;
  int gy = pos.y * kChunkDim;

  if (pos.y < 0) {
    return;
  }

  int numBlocks = static_cast<int>(context_.blocks_->block_type_data_.size());

  int RowLen = 50;
  int ColLen = 50;

  if (gy == 0) {
    for (auto [x, z] : Product<2>(kChunkDim)) {
      chunk->SetBlockUnsafe(context_.blocks_->WHITE_CONCRETE, {x, 0, z});
    }
  }

  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    int px = x + gx;
    int pz = z + gz;
    int py = y + gy - 1;

    if (((px & 0b1) == 1) || ((pz & 0b1) == 1) || ((py & 0b1) == 1)) {
      continue;
    }

    px = px / 2;
    pz = pz / 2;
    py = py / 2;

    if ((px < 0) || (px >= RowLen) || (pz < 0) || (pz >= ColLen)) {
      continue;
    }

    int b = px + pz * RowLen + py * RowLen * ColLen;

    if ((b < numBlocks) && (b >= 0)) {
      chunk->SetBlockUnsafe(b, {x, y, z});
    }
  }
}

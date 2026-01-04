#include "Level/TerrainGeneration/Generators/DebugMeshStress/DebugMeshStress.h"

#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/Generator.h"
#include "Utils/Iterators.h"

using std::unique_ptr;

DebugMeshStress::DebugMeshStress(GameContext& context)
    : WorldGenerator{context} {}
DebugMeshStress::~DebugMeshStress() = default;

void DebugMeshStress::Generate(ChunkPos pos, unique_ptr<Chunk>& chunk) {
  int gx = pos.x * kChunkDim;
  int gz = pos.z * kChunkDim;
  int gy = pos.y * kChunkDim;

  if (pos.y < 0) {
    return;
  }

  int numBlocks = static_cast<int>(context_.blocks_->block_type_data_.size());

  if (gy == 0) {
    for (auto [x, z] : Product<2>(kChunkDim)) {
      chunk->SetBlockUnsafe(context_.blocks_->WHITE_CONCRETE, {x, 0, z});
    }
  }

  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    int px = x;
    int pz = z;
    int py = y;

    if (((x & 0b1) == 1) || ((z & 0b1) == 1) || ((y & 0b1) == 1)) {
      continue;
    }

    px = px / 2;
    pz = pz / 2;
    py = py / 2;

    static constexpr int kChunkDimHalf = kChunkDim / 2;
    int idx = py * kChunkDimHalf * kChunkDimHalf + kChunkDimHalf * px + pz;

    if ((idx < numBlocks)) {
      chunk->SetBlockUnsafe(idx, {x, y, z});
    }
  }
}

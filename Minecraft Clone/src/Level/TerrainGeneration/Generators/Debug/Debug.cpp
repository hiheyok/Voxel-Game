#include "Level/TerrainGeneration/Generators/Debug/Debug.h"

#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/Generator.h"
#include "Utils/Iterators.h"

using std::unique_ptr;

DebugWorld::DebugWorld(GameContext& context) : WorldGenerator{context} {}
DebugWorld::~DebugWorld() = default;

void DebugWorld::Generate(ChunkPos pos, unique_ptr<Chunk>& chunk) {
  int gx = pos.x * kChunkDim;
  int gz = pos.z * kChunkDim;
  int gy = pos.y * kChunkDim;

  if (pos.y < -1) {
    return;
  }

  if (pos.y == -1) {
    int parity = (pos.z ^ pos.x) & 1;
    BlockID block = parity ? context_.blocks_->LIGHT_GRAY_CONCRETE
                           : context_.blocks_->WHITE_CONCRETE;
    for (auto [x, z] : Product<2>(kChunkDim)) {
      chunk->SetBlockUnsafe(block, {x, 15, z});
    }
    return;
  }

  int numBlocks = static_cast<int>(context_.blocks_->block_type_data_.size());

  int RowLen = 50;
  int ColLen = 50;

  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    int px = x + gx;
    int pz = z + gz;
    int py = y + gy;

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

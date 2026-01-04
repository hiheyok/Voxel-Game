#include "Level/TerrainGeneration/Generators/DebugPositioning/DebugPositioning.h"

#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/Generator.h"

DebugPositioning::DebugPositioning(GameContext& context)
    : WorldGenerator{context} {}
DebugPositioning::~DebugPositioning() = default;

void DebugPositioning::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  (void)pos;
  // Red -> x
  ChunkPos newPos = chunk->position_;
  for (int i = 0; i < kChunkDim; ++i) {
    if (newPos.x & 1)
      chunk->SetBlockUnsafe(context_.blocks_->RED_WOOL, {i, 0, 0});
    if (newPos.y & 1)
      chunk->SetBlockUnsafe(context_.blocks_->GREEN_WOOL, {i, 1, 0});
    if (newPos.z & 1)
      chunk->SetBlockUnsafe(context_.blocks_->BLUE_WOOL, {i, 2, 0});

    newPos.x >>= 1;
    newPos.y >>= 1;
    newPos.z >>= 1;
  }

  chunk->SetBlockUnsafe(context_.blocks_->RED_WOOL, {1, 4, 0});
  chunk->SetBlockUnsafe(context_.blocks_->GREEN_WOOL, {0, 5, 0});
  chunk->SetBlockUnsafe(context_.blocks_->BLUE_WOOL, {0, 4, 1});

  chunk->SetBlockUnsafe(context_.blocks_->GOLD_BLOCK, {0, 3, 0});
  chunk->SetBlockUnsafe(context_.blocks_->EMERALD_BLOCK, {15, 3, 0});

  for (int i = 1; i < kChunkDim - 1; ++i) {
    if (i & 1) {
      chunk->SetBlockUnsafe(context_.blocks_->BLACK_CONCRETE, {i, 3, 0});
    } else {
      chunk->SetBlockUnsafe(context_.blocks_->WHITE_CONCRETE, {i, 3, 0});
    }
  }
}

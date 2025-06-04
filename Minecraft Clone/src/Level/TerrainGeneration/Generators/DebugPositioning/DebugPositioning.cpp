#include "Level/TerrainGeneration/Generators/DebugPositioning/DebugPositioning.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"

DebugPositioning::DebugPositioning(GameContext& game_context)
    : WorldGenerator{game_context} {}
DebugPositioning::~DebugPositioning() = default;

void DebugPositioning::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  (void)pos;
  // Red -> x
  ChunkPos newPos = chunk->position_;
  for (int i = 0; i < kChunkDim; ++i) {
    if (newPos.x & 1)
      chunk->SetBlockUnsafe(game_context_.blocks_->RED_WOOL, BlockPos{i, 0, 0});
    if (newPos.y & 1)
      chunk->SetBlockUnsafe(game_context_.blocks_->GREEN_WOOL,
                            BlockPos{i, 1, 0});
    if (newPos.z & 1)
      chunk->SetBlockUnsafe(game_context_.blocks_->BLUE_WOOL,
                            BlockPos{i, 2, 0});

    newPos.x >>= 1;
    newPos.y >>= 1;
    newPos.z >>= 1;
  }

  chunk->SetBlockUnsafe(game_context_.blocks_->RED_WOOL, BlockPos{1, 4, 0});
  chunk->SetBlockUnsafe(game_context_.blocks_->GREEN_WOOL, BlockPos{0, 5, 0});
  chunk->SetBlockUnsafe(game_context_.blocks_->BLUE_WOOL, BlockPos{0, 4, 1});

  chunk->SetBlockUnsafe(game_context_.blocks_->GOLD_BLOCK, BlockPos{0, 3, 0});
  chunk->SetBlockUnsafe(game_context_.blocks_->EMERALD_BLOCK,
                        BlockPos{15, 3, 0});

  for (int i = 1; i < kChunkDim - 1; ++i) {
    if (i & 1) {
      chunk->SetBlockUnsafe(game_context_.blocks_->BLACK_CONCRETE,
                            BlockPos{i, 3, 0});
    } else {
      chunk->SetBlockUnsafe(game_context_.blocks_->WHITE_CONCRETE,
                            BlockPos{i, 3, 0});
    }
  }
}

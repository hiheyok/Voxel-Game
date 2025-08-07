#include "Level/TerrainGeneration/Generators/DebugSecond/DebugSecond.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"

DebugWorldSecond::DebugWorldSecond(GameContext& context)
    : WorldGenerator{context} {}
DebugWorldSecond::~DebugWorldSecond() = default;

void DebugWorldSecond::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  chunk->SetBlock(context_.blocks_->COBBLESTONE, BlockPos{8, 8, 8});

  BlockPos scaled_pos = BlockPos{pos.x, pos.y, pos.z} * kChunkDim;

  BlockPos local_pos{0, 0, 0};

  for (local_pos.x = 0; local_pos.x < kChunkDim; ++local_pos.x) {
    for (local_pos.y = 0; local_pos.y < kChunkDim; ++local_pos.y) {
      for (local_pos.z = 0; local_pos.z < kChunkDim; ++local_pos.z) {
        int index = local_pos.GetIndex();

        if (index % 5) {
          continue;
        }

        BlockPos global_pos = local_pos + scaled_pos;

        if ((abs(global_pos.x) >= abs(global_pos.z)) &&
            (abs(global_pos.x) >= abs(global_pos.y))) {
          chunk->SetBlock(context_.blocks_->BLUE_STAINED_GLASS, local_pos);
        }
        if ((abs(global_pos.z) >= abs(global_pos.x)) &&
            (abs(global_pos.z) >= abs(global_pos.y))) {
          chunk->SetBlock(context_.blocks_->DARK_OAK_PLANKS, local_pos);
        }
        if ((abs(global_pos.y) >= abs(global_pos.x)) &&
            (abs(global_pos.y) >= abs(global_pos.z))) {
          chunk->SetBlock(context_.blocks_->ORANGE_CONCRETE, local_pos);
        }
      }
    }
  }
}

#include "Level/TerrainGeneration/Generators/Superflat/Superflat.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"

SuperflatWorld::SuperflatWorld(GameContext& context)
    : WorldGenerator{context} {}
SuperflatWorld::~SuperflatWorld() = default;

void SuperflatWorld::Generate(ChunkPos pos, std::unique_ptr<Chunk>& chunk) {
  int cy = pos.y * kChunkDim;
  for (auto [x, y, z] : Product<3>(kChunkDim)) {
    BlockPos block_pos{x, y, z};
    if (block_pos.y + cy < 10) {
      chunk->SetBlockUnsafe(context_.blocks_->WHITE_CONCRETE, block_pos);
    }
  }
}

#include "Level/TerrainGeneration/Generators/DebugMeshStressFace/DebugMeshStressFace.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"

DebugMeshStressFace::DebugMeshStressFace(GameContext& context)
    : WorldGenerator{context} {}
DebugMeshStressFace::~DebugMeshStressFace() = default;

void DebugMeshStressFace::Generate(ChunkPos pos,
                                   std::unique_ptr<Chunk>& chunk) {
  (void)pos;
  for (auto [x, y, z] : Product<3>(0, kChunkDim)) {
    if ((y & 1)) {
      chunk->SetBlockUnsafe(context_.blocks_->WHITE_CONCRETE, {x, y, z});
    }
  }
}

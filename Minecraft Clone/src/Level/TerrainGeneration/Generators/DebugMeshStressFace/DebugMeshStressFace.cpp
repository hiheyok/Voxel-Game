#include "Level/TerrainGeneration/Generators/DebugMeshStressFace/DebugMeshStressFace.h"

#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/TerrainGeneration/Generators/Generator.h"
#include "Utils/Iterators.h"

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

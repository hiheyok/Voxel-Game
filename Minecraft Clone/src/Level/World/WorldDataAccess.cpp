#include <exception>

#include "WorldDataAccess.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/ChunkMap.h"
#include "Level/Container/EntityContainer.h"

WorldAccess::WorldAccess() :
    chunk_container_{ std::make_unique<ChunkMap>(true) },
    entities_{ std::make_unique<EntityContainer>() } {
}

WorldAccess::~WorldAccess() = default;

BlockID WorldAccess::GetBlock(const BlockPos& pos) {
    return chunk_container_->GetBlock(pos);
}

void WorldAccess::SetBlock(BlockID block, const BlockPos& pos) {
    if (!chunk_container_->SetBlock(block, pos)) {
        g_logger.LogError("WorldAccess::SetBlock", "Tried to place block outside of the world");
    }
}

void WorldAccess::SetChunk(std::unique_ptr<Chunk> chunk) {
    chunk_container_->InsertChunk(std::move(chunk));
}

Chunk* WorldAccess::GetChunk(const ChunkPos& pos) const {
    return chunk_container_->GetChunk(pos);
}

bool WorldAccess::CheckChunk(const ChunkPos& pos) const {
    return chunk_container_->CheckChunk(pos);
}
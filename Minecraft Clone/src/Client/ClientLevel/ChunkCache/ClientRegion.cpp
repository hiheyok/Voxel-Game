#include "Client/ClientLevel/ChunkCache/ClientRegion.h"
#include "Level/Chunk/Chunk.h"

ClientRegion::ClientRegion() {
    region_data_.resize(kRegionSize3D);
}

ClientRegion::~ClientRegion() = default;

Chunk* ClientRegion::GetChunk(const ChunkPos& pos) const {
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim + localChunkPos.z;
    if (!CheckChunk(pos)) {
        g_logger.LogError("ClientRegion::GetChunk", "Tried to get non-existent chunk!");
    }
    return region_data_[idx].get();
}

bool ClientRegion::CheckChunk(const ChunkPos& pos) const {
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim + localChunkPos.z;
    return region_data_[idx] != nullptr;
}

void ClientRegion::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos& pos = chunk->position_;
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim + localChunkPos.z;
    if (region_data_[idx] != nullptr) {
        g_logger.LogWarn("ClientRegion::InsertChunk", "Chunk already exist! Replacing chunk"); // TODO: Maybe change this to error later
        chunk_count_--;
    }
    chunk_count_++;
    region_data_[idx] = std::move(chunk);
}

void ClientRegion::EraseChunk(const ChunkPos& pos) {
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim + localChunkPos.z;
    if (region_data_[idx] == nullptr) {
        g_logger.LogWarn("ClientRegion::EraseChunk", "Chunk doesn't exist!"); // TODO: Maybe change this to error later
    }
    else {
        chunk_count_--;
        region_data_[idx] = nullptr;
    }
}

int ClientRegion::GetChunkCount() const {
    return chunk_count_;
}
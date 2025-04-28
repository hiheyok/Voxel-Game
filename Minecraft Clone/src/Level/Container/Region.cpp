#include "Level/Container/Region.h"

#include <exception>

#include "Level/Chunk/Chunk.h"

Region::Region() {
    chunk_count_ = 0;
    usage_ = 0;
    region_data_.resize(kRegionSize3D);
}

Region::~Region() = default;

Chunk* Region::GetChunk(const ChunkPos& pos) const {
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim +
              localChunkPos.z;
    if (!CheckChunk(pos)) {
        g_logger.LogError("ClientRegion::GetChunk",
                          "Tried to get non-existent chunk!");
    }
    return region_data_[idx].get();
}

bool Region::CheckChunk(const ChunkPos& pos) const {
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim +
              localChunkPos.z;
    return region_data_[idx] != nullptr;
}

void Region::InsertChunk(std::unique_ptr<Chunk> chunk) {
    const ChunkPos& pos = chunk->position_;
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim +
              localChunkPos.z;
    if (region_data_[idx] != nullptr) {
        g_logger.LogWarn(
            "ClientRegion::InsertChunk",
            "Chunk already exist! Replacing chunk");  // TODO: Maybe change this
                                                      // to error later
        chunk_count_--;
    }
    chunk_count_++;
    region_data_[idx] = std::move(chunk);
}

void Region::EraseChunk(const ChunkPos& pos) {
    ChunkPos localChunkPos = pos & (kRegionDim - 1);
    int idx = localChunkPos.x * kRegionSize2D + localChunkPos.y * kRegionDim +
              localChunkPos.z;
    if (region_data_[idx] == nullptr) {
        g_logger.LogWarn(
            "ClientRegion::EraseChunk",
            "Chunk doesn't exist!");  // TODO: Maybe change this to error later
    } else {
        chunk_count_--;
        region_data_[idx] = nullptr;
    }
}

int Region::GetChunkCount() const { return chunk_count_; }

void Region::IncrementUsage() { usage_++; }

size_t Region::GetUsageCount() const { return usage_; }

void Region::ResetUsageCount() { usage_ = 0; }
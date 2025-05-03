// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Container/Region.h"

#include <exception>

#include "Level/Chunk/Chunk.h"

Region::Region() {
  chunk_count_ = 0;
  usage_ = 0;
  region_data_.resize(kRegionSize3D);
}

Region::~Region() = default;

Chunk* Region::GetChunk(ChunkPos pos) const {
  int idx = pos.GetIndex();
  if (!CheckChunk(pos)) {
    g_logger.LogError("ClientRegion::GetChunk",
                      "Tried to get non-existent chunk!");
  }
  return region_data_[idx].get();
}

bool Region::CheckChunk(ChunkPos pos) const {
  int idx = pos.GetIndex();
  return region_data_[idx] != nullptr;
}

void Region::InsertChunk(std::unique_ptr<Chunk> chunk) {
  ChunkPos pos = chunk->position_;
  int idx = pos.GetIndex();
  if (region_data_[idx] != nullptr) {
    g_logger.LogWarn(
        "ClientRegion::InsertChunk",
        "Chunk already exist! Replacing chunk");  // TODO(hiheyok): Maybe change
                                                  // this to error later
    chunk_count_--;
  }
  chunk_count_++;
  region_data_[idx] = std::move(chunk);
}

void Region::EraseChunk(ChunkPos pos) {
  int idx = pos.GetIndex();
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

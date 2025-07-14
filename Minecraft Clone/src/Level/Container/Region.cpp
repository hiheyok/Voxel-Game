// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Container/Region.h"

#include <exception>

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Utils/LogUtils.h"

Region::Region(GameContext& game_context) : game_context_{game_context} {
  chunk_count_ = 0;
  usage_ = 0;
  region_data_.resize(kRegionSize3D);
}

Region::~Region() = default;

Chunk* Region::GetChunk(ChunkPos pos) const {
  int idx = pos.GetIndex();
  assert(CheckChunk(pos));
  return region_data_[idx].get();
}

bool Region::CheckChunk(ChunkPos pos) const noexcept {
  int idx = pos.GetIndex();
  return region_data_[idx] != nullptr;
}

void Region::InsertChunk(std::unique_ptr<Chunk> chunk) {
  ChunkPos pos = chunk->position_;
  int idx = pos.GetIndex();
  assert(region_data_[idx] == nullptr);
  chunk_count_++;
  region_data_[idx] = std::move(chunk);
}

void Region::EraseChunk(ChunkPos pos) {
  int idx = pos.GetIndex();
  assert(region_data_[idx] != nullptr);
  chunk_count_--;
  region_data_[idx] = nullptr;
}

int Region::GetChunkCount() const noexcept { return chunk_count_; }

void Region::IncrementUsage() noexcept { usage_++; }

size_t Region::GetUsageCount() const noexcept { return usage_; }

void Region::ResetUsageCount() noexcept { usage_ = 0; }

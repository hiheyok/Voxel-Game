// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Container/Region.h"

#include <cstddef>
#include <memory>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Chunk/Chunk.h"
#include "Utils/Assert.h"

Region::Region(GameContext& context)
    : context_{context}, region_data_(kRegionSize3D) {
  chunk_count_ = 0;
  usage_ = 0;
}

Region::~Region() = default;

Chunk& Region::GetChunk(ChunkPos pos) const noexcept {
  int idx = pos.GetIndex();
  GAME_ASSERT(CheckChunk(pos), "Chunk not in region");
  return *region_data_[idx];
}

bool Region::CheckChunk(ChunkPos pos) const noexcept {
  int idx = pos.GetIndex();
  return region_data_[idx] != nullptr;
}

void Region::InsertChunk(std::unique_ptr<Chunk> chunk) noexcept {
  ChunkPos pos = chunk->position_;
  int idx = pos.GetIndex();
  GAME_ASSERT(region_data_[idx] == nullptr, "Chunk already exists in region");
  chunk_count_++;
  region_data_[idx] = std::move(chunk);
}

void Region::EraseChunk(ChunkPos pos) noexcept {
  int idx = pos.GetIndex();
  GAME_ASSERT(region_data_[idx] != nullptr, "Chunk does not exist in region");
  chunk_count_--;
  region_data_[idx] = nullptr;
}

int Region::GetChunkCount() const noexcept { return chunk_count_; }

void Region::IncrementUsage() noexcept { usage_++; }

size_t Region::GetUsageCount() const noexcept { return usage_; }

void Region::ResetUsageCount() noexcept { usage_ = 0; }

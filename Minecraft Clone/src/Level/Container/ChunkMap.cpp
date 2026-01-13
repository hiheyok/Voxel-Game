// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Container/ChunkMap.h"

#include <atomic>
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Container/Region.h"
#include "Utils/Assert.h"

using std::make_unique;
using std::move;
using std::unique_ptr;
using std::vector;

ChunkMap::ChunkMap(GameContext& context, bool neighborUpdate,
                   bool heightmapUpdate)
    : context_{context},
      heightmap_update_{heightmapUpdate},
      neighbor_update_{neighborUpdate} {}

ChunkMap::~ChunkMap() = default;

void ChunkMap::EraseChunk(ChunkPos pos) {
  GAME_ASSERT(CheckChunk(pos), "Chunk not in map");
  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegion(reg_pos);

  for (auto offset : Directions<ChunkPos>()) {
    ChunkPos neighborPos = pos + offset;

    if (CheckChunk(neighborPos)) {
      GetChunk(neighborPos).SetNeighbor(nullptr, !offset);
    }
  }

  // erase chunk from chunks
  Chunk* swapped_chunk = chunks_.back();

  int idx = chunks_idx_[pos];
  chunks_idx_[swapped_chunk->position_] = idx;
  chunks_idx_.erase(pos);
  std::swap(chunks_[idx], chunks_.back());
  chunks_.pop_back();

  auto bottom_chunk =
      reg->GetChunk(pos).GetNeighbor(Directions<ChunkPos>::kDown);

  reg->EraseChunk(pos);

  if (heightmap_update_ && bottom_chunk.has_value()) {
    bottom_chunk.value()->UpdateHeightMap();
  }

  // Delete region if it is empty
  if (reg->GetChunkCount() == 0) {
    DeleteRegion(reg_pos);
  }
}

bool ChunkMap::SetBlock(BlockID block, BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  RegionPos reg_pos = pos.ToRegionPos();
  BlockPos local_pos = pos.GetLocalPos();

  Region* reg = GetRegionUncheck(reg_pos);
  if (reg == nullptr) return false;
  if (reg->CheckChunk(chunk_pos)) {
    Chunk& chunk = reg->GetChunk(chunk_pos);
    chunk.SetBlockUnsafe(block, local_pos);
    chunk.is_empty_ = false;

    if (heightmap_update_) {
      chunk.UpdateHeightMap(local_pos.x, local_pos.z);
    }

    return true;
  }

  return false;
}

BlockID ChunkMap::GetBlock(BlockPos pos) const {
  ChunkPos chunk_pos = pos.ToChunkPos();
  RegionPos reg_pos = pos.ToRegionPos();
  BlockPos local_block_pos = pos.GetLocalPos();

  Region* reg = GetRegionUncheck(reg_pos);
  if (reg == nullptr) return context_.blocks_->AIR;

  if (!reg->CheckChunk(chunk_pos)) {
    return context_.blocks_->AIR;
  }

  return reg->GetChunk(chunk_pos).GetBlock(local_block_pos);
}

bool ChunkMap::CheckChunk(ChunkPos pos) const {
  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegionUncheck(reg_pos);
  if (reg == nullptr) return false;
  return reg->CheckChunk(pos);
}

Chunk& ChunkMap::GetChunk(ChunkPos pos) const {
  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegionUncheck(reg_pos);
  GAME_ASSERT(reg != nullptr, "Chunk not in map");
  return reg->GetChunk(pos);
}

const vector<Chunk*>& ChunkMap::GetAllChunks() const { return chunks_; }

void ChunkMap::InsertChunk(unique_ptr<Chunk> chunk) {
  const ChunkPos pos = chunk->position_;

  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegionUncheck(reg_pos);

  if (reg == nullptr) {
    reg = CreateRegion(reg_pos);
  }

  for (auto offset : Directions<ChunkPos>()) {
    ChunkPos neigh_pos = chunk->position_ + offset;

    if (CheckChunk(neigh_pos)) {
      chunk->SetNeighbor(&GetChunk(neigh_pos), offset);
      GetChunk(neigh_pos).SetNeighbor(chunk.get(), !offset);
      if (neighbor_update_) {
        GetChunk(neigh_pos).UpdateGen();
      }
    }
  }

  chunk->UpdateGen();

  if (reg->CheckChunk(pos)) {
    int idx = chunks_idx_[pos];
    chunks_[idx] = chunk.get();
    reg->InsertChunk(move(chunk));
  } else {
    chunks_.push_back(chunk.get());
    chunks_idx_.emplace(chunk->position_, chunks_.size() - 1);
    reg->InsertChunk(move(chunk));
  }

  Chunk& chunkRef = reg->GetChunk(pos);

  if (heightmap_update_) {
    chunkRef.UpdateHeightMap();
  }
}

Region* ChunkMap::GetRegion(RegionPos pos) const noexcept {
  Region* region = FindAndCacheRegion(pos);
  GAME_ASSERT(region != nullptr, "Region doesn't exist");
  return region;
}

Region* ChunkMap::GetRegionUncheck(RegionPos pos) const noexcept {
  // First try to find region in the cache
  return FindAndCacheRegion(pos);
}

bool ChunkMap::CheckRegion(RegionPos pos) const noexcept {
  Region* reg = FindAndCacheRegion(pos);
  return reg != nullptr;
}

Region* ChunkMap::CreateRegion(RegionPos pos) {
  GAME_ASSERT(!CheckRegion(pos),
              "Tried to create region when one already exists");
  unique_ptr<Region> newRegion = make_unique<Region>(context_);
  Region* region = newRegion.get();
  regions_.emplace(pos, move(newRegion));
  ++epoch_;  // Invalidate all thread-local caches
  return region;
}

void ChunkMap::DeleteRegion(RegionPos pos) noexcept {
  GAME_ASSERT(CheckRegion(pos), "Region doesn't exist");
  ++epoch_;  // Invalidate all thread-local caches
  regions_.erase(pos);
}

Region* ChunkMap::FindAndCacheRegion(RegionPos pos) const noexcept {
  struct alignas(32) ThreadLocalCache {
    const ChunkMap* map_ = nullptr;
    RegionPos pos_{};
    Region* region_ = nullptr;
    uint32_t epoch_ = 0;
  };
  thread_local ThreadLocalCache tl_cache;

  uint32_t current_epoch = epoch_.load(std::memory_order_acquire);

  if (tl_cache.map_ == this && tl_cache.epoch_ == current_epoch &&
      tl_cache.pos_ == pos) {
    return tl_cache.region_;
  }

  auto it = regions_.find(pos);
  if (it == regions_.end()) {
    return nullptr;
  }

  Region* region = it->second.get();

  // Update thread-local cache
  tl_cache.map_ = this;
  tl_cache.pos_ = pos;
  tl_cache.region_ = region;
  tl_cache.epoch_ = current_epoch;

  return region;
}

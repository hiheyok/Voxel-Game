// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Container/ChunkMap.h"

#include <cassert>
#include <limits>
#include <optional>
#include <stdexcept>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Container/Region.h"
#include "Utils/Assert.h"

ChunkMap::ChunkMap(GameContext& context, bool neighborUpdate,
                   bool heightmapUpdate)
    : context_{context},
      heightmap_update_{heightmapUpdate},
      neighbor_update_{neighborUpdate} {
  region_cache_.reserve(kRegionCacheSize);
}
ChunkMap::~ChunkMap() = default;

void ChunkMap::EraseChunk(ChunkPos pos) {
  GAME_ASSERT(CheckChunk(pos), "Chunk not in map");
  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegion(reg_pos);

  for (auto offset : Directions<ChunkPos>()) {
    ChunkPos neighborPos = pos + offset;

    if (CheckChunk(neighborPos)) {
      GetChunk(neighborPos)->SetNeighbor(nullptr, !offset);
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
      reg->GetChunk(pos)->GetNeighbor(Directions<ChunkPos>::kDown);

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

  Chunk* chunk = reg->GetChunk(chunk_pos);

  if (chunk != nullptr) {
    chunk->SetBlockUnsafe(block, local_pos);
    chunk->is_empty_ = false;

    if (heightmap_update_) {
      chunk->UpdateHeightMap(local_pos.x, local_pos.z);
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

  Chunk* chunk = reg->GetChunk(chunk_pos);

  return chunk->GetBlock(local_block_pos);
}

bool ChunkMap::CheckChunk(ChunkPos pos) const {
  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegionUncheck(reg_pos);
  if (reg == nullptr) return false;
  return reg->CheckChunk(pos);
}

Chunk* ChunkMap::GetChunk(ChunkPos pos) const {
  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegionUncheck(reg_pos);
  if (reg == nullptr) return nullptr;

  return reg->GetChunk(pos);
}

std::vector<Chunk*> ChunkMap::GetAllChunks() const { return chunks_; }

void ChunkMap::InsertChunk(std::unique_ptr<Chunk> chunk) {
  const ChunkPos pos = chunk->position_;

  RegionPos reg_pos = pos.ToRegionPos();
  Region* reg = GetRegionUncheck(reg_pos);

  if (reg == nullptr) {
    reg = CreateRegion(reg_pos);
  }

  for (auto offset : Directions<ChunkPos>()) {
    ChunkPos neigh_pos = chunk->position_ + offset;

    if (CheckChunk(neigh_pos)) {
      chunk->SetNeighbor(static_cast<ChunkContainer*>(GetChunk(neigh_pos)),
                         offset);
      GetChunk(neigh_pos)->SetNeighbor(
          static_cast<ChunkContainer*>(chunk.get()), !offset);
      if (neighbor_update_) {
        GetChunk(neigh_pos)->UpdateGen();
      }
    }
  }

  chunk->UpdateGen();

  if (reg->CheckChunk(pos)) {
    int idx = chunks_idx_[pos];
    chunks_[idx] = chunk.get();
    reg->InsertChunk(std::move(chunk));
  } else {
    chunks_.push_back(chunk.get());
    chunks_idx_.emplace(chunk->position_, chunks_.size() - 1);
    reg->InsertChunk(std::move(chunk));
  }

  Chunk* chunkPtr = reg->GetChunk(pos);

  if (heightmap_update_) {
    chunkPtr->UpdateHeightMap();
  }
}

Region* ChunkMap::GetRegion(RegionPos pos) const {
  Region* region = FindAndCacheRegion(pos);
  GAME_ASSERT(region != nullptr, "Region doesn't exist");
  return region;
}

Region* ChunkMap::GetRegionUncheck(RegionPos pos) const {
  // First try to find region in the cache
  return FindAndCacheRegion(pos);
}

bool ChunkMap::CheckRegion(RegionPos pos, bool check_cache) const {
  if (check_cache) {
    Region* reg = FindAndCacheRegion(pos);
    return reg != nullptr;
  } else {
    return regions_.contains(pos);
  }
}

Region* ChunkMap::CreateRegion(RegionPos pos) {
  GAME_ASSERT(!CheckRegion(pos), "Tried to create region when one already exists");
  std::unique_ptr<Region> newRegion = std::make_unique<Region>(context_);
  Region* region = newRegion.get();
  regions_.emplace(pos, std::move(newRegion));
  return region;
}

void ChunkMap::DeleteRegion(RegionPos pos) {
  GAME_ASSERT(CheckRegion(pos), "Region doesn't exist");
  TryDeleteFromCache(pos);
  regions_.erase(pos);
}

Region* ChunkMap::FindAndCacheRegion(RegionPos pos) const {
  // First try to find region in the cache
  size_t minUsageCount = SIZE_MAX;
  int minIdx = -1;

  for (size_t i = 0; i < region_cache_.size(); ++i) {
    Region* currRegion = region_cache_[i].second;

    if (region_cache_[i].first == pos) {
      currRegion->IncrementUsage();
      return currRegion;
    }
    
    size_t currUsage = currRegion->GetUsageCount();
    if (minUsageCount > currUsage) {
      minUsageCount = currUsage;
      minIdx = static_cast<int>(i);
    }
  }

  // Find the region in main if its not in cache
  const auto& it = regions_.find(pos);

  if (it == regions_.end()) {
    return nullptr;
  }

  Region* region = it->second.get();

  // Cache region
  if (region_cache_.size() < kRegionCacheSize) {
    region_cache_.emplace_back(pos, region);
    return region;
  }

  // Cache is full, replace the least-used entry (already found in first loop)
  region_cache_[minIdx] = {pos, region};

  return region;
}

void ChunkMap::TryDeleteFromCache(RegionPos pos) {
  for (size_t i = 0; i < region_cache_.size(); ++i) {
    if (region_cache_[i].first == pos) {
      std::swap(region_cache_[i], region_cache_.back());
      region_cache_.pop_back();
      break;
    }
  }
}

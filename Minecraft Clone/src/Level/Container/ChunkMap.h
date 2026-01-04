// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <atomic>
#include <memory>
#include <utility>

class Chunk;
class Region;
class GameContext;

#include "Core/Typenames.h"

class ChunkMap {
 public:
  ChunkMap(GameContext&, bool neighbor_update = false,
           bool heightmap_update = false);
  ~ChunkMap();

  // Prevent copying/moving which would cause cache issues
  ChunkMap(const ChunkMap&) = delete;
  ChunkMap& operator=(const ChunkMap&) = delete;
  ChunkMap(ChunkMap&&) = delete;
  ChunkMap& operator=(ChunkMap&&) = delete;

  BlockID GetBlock(BlockPos pos) const;
  bool SetBlock(BlockID block, BlockPos pos);

  bool CheckChunk(ChunkPos pos) const;
  void InsertChunk(std::unique_ptr<Chunk> chunk);
  void EraseChunk(ChunkPos pos);
  Chunk& GetChunk(ChunkPos pos) const;
  std::vector<Chunk*> GetAllChunks() const;

 private:
  Region* GetRegion(RegionPos pos) const noexcept;
  Region* GetRegionUncheck(RegionPos pos) const noexcept;
  bool CheckRegion(RegionPos pos) const noexcept;
  Region* CreateRegion(RegionPos pos);
  void DeleteRegion(RegionPos pos) noexcept;
  Region* FindAndCacheRegion(RegionPos pos) const noexcept;

  GameContext& context_;
  
  bool heightmap_update_ = false;
  bool neighbor_update_ = false;

  std::vector<Chunk*> chunks_;
  FastHashMap<ChunkPos, size_t> chunks_idx_;
  FastHashMap<RegionPos, std::unique_ptr<Region>> regions_;

  // Epoch counter for cache invalidation - incremented on any region modification
  mutable std::atomic<uint64_t> epoch_{0};
};

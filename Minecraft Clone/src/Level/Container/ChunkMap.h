// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <optional>
#include <utility>

class Chunk;
class Region;
class GameContext;

#include "Core/Typenames.h"

class ChunkMap {
 public:
  ChunkMap(GameContext&, bool neighborUpdate = false, bool heightmapUpdate = false);
  ~ChunkMap();

  BlockID GetBlock(BlockPos pos) const;
  bool SetBlock(BlockID block, BlockPos pos);

  bool CheckChunk(ChunkPos pos) const;
  void InsertChunk(std::unique_ptr<Chunk> chunk);
  void EraseChunk(ChunkPos pos);
  Chunk* GetChunk(ChunkPos pos) const;
  std::vector<Chunk*> GetAllChunks() const;

 private:
  Region* GetRegion(RegionPos pos) const;
  Region* GetRegionUncheck(RegionPos pos) const;
  bool CheckRegion(RegionPos pos, bool checkCache = true) const;
  Region* CreateRegion(RegionPos pos);
  void DeleteRegion(RegionPos pos);
  Region* FindAndCacheRegion(RegionPos pos) const;
  void TryDeleteFromCache(RegionPos pos);
  
  GameContext& game_context_;
  bool heightmap_update_ = false;
  bool neighbor_update_ = false;

  static constexpr int kRegionCacheSize = 16;

  std::vector<Chunk*> chunks_;
  FastHashMap<ChunkPos, size_t> chunks_idx_;
  FastHashMap<RegionPos, std::unique_ptr<Region>> regions_;
  mutable std::vector<std::pair<RegionPos, Region*>> region_cache_;
};

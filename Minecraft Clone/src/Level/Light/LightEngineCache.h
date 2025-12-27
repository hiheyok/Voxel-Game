#pragma once

#include "Core/Typenames.h"

class WorldInterface;
class GameContext;
class LightStorage;
class Chunk;

/*
Cache chunks and light data
Gives engine direct access for fast access
Cache 3 x 3 x H sections, H >= 5
For skylight, it can also dynamically add cache layers
*/
class LightEngineCache {
 public:
  struct LightEngineCacheStats {
    int set_block_light_ = 0;
    int set_sky_light_ = 0;
    int get_block_light_ = 0;
    int get_sky_light_ = 0;
    int get_block_ = 0;
    int get_chunk_ = 0;
    int check_chunk_ = 0;
    int try_cache_chunk_ = 0;
  };

  LightEngineCache(GameContext&, WorldInterface&);

  // Set up 3x3x3 cache first with the center one being the starting chunk
  void BuildCache(BlockPos center);
  void ResetCache();

  int GetBlockLight(BlockPos pos);
  int GetSkyLight(BlockPos pos);
  BlockID GetBlock(BlockPos pos);

  void SetBlockLight(BlockPos pos, int lvl);
  void SetSkyLight(BlockPos pos, int lvl);

  bool CheckChunk(ChunkPos pos);
  bool CheckChunk(BlockPos pos);
  Chunk* GetChunk(ChunkPos pos);
  Chunk* GetChunk(BlockPos pos);

  void UpdateLight();
  bool CheckChunkHasLighting(ChunkPos pos);
  LightEngineCacheStats GetStats() const noexcept;

 private:
  class alignas(16) CacheSlot {
   public:
    Chunk* GetChunk() const {
      // Cast the integer bits back to a pointer
      return reinterpret_cast<Chunk*>(chunk_);
    }

    LightStorage* GetBlock() const {
      return reinterpret_cast<LightStorage*>(block_);
    }

    LightStorage* GetSky() const {
      return reinterpret_cast<LightStorage*>(sky_);
    }

    bool IsDirty() const { return dirty_; }

    void SetChunk(Chunk* chunk) { chunk_ = reinterpret_cast<uintptr_t>(chunk); }

    void SetBlock(LightStorage* block) {
      block_ = reinterpret_cast<uintptr_t>(block);
    }

    void SetSky(LightStorage* sky) { sky_ = reinterpret_cast<uintptr_t>(sky); }

    void SetDirty(bool is_dirty) { dirty_ = is_dirty; }

   private:
    bool dirty_ : 1;
    uintptr_t chunk_ : 48;
    uintptr_t block_ : 48;
    uintptr_t sky_ : 48;
  };

  void SetBlockCache(ChunkPos pos, Chunk* data);
  void SetBlockLightCache(ChunkPos pos, LightStorage* data);
  void SetSkyLightCache(ChunkPos pos, LightStorage* data);
  void ExpandDown();
  bool TryCacheChunk(ChunkPos pos);
  size_t CalculateCacheIndex(ChunkPos pos) const noexcept;
  // Calculate the index for the chunk the block_pos is in
  size_t CalculateCacheIndex(BlockPos pos) const noexcept;
  [[nodiscard]] bool EnsureLoaded(ChunkPos pos);
  [[nodiscard]] LightStorage* EnsureLoadedGetSky(ChunkPos pos);
  [[nodiscard]] LightStorage* EnsureLoadedGetSky(BlockPos pos);
  [[nodiscard]] LightStorage* EnsureLoadedGetBlock(ChunkPos pos);
  [[nodiscard]] LightStorage* EnsureLoadedGetBlock(BlockPos pos);
  [[nodiscard]] Chunk* EnsureLoadedGetChunk(ChunkPos pos);
  [[nodiscard]] Chunk* EnsureLoadedGetChunk(BlockPos pos);

  static constexpr int kCacheRadius = 2;
  static constexpr int kCacheWidth = 1 + 2 * kCacheRadius;
  static constexpr int kCacheSlice = kCacheWidth * kCacheWidth;

  GameContext& context_;
  WorldInterface& world_;

  ChunkPos center_chunk_;
  BlockPos center_block_offset_;
  int y_top;
  int y_bot;

  std::vector<CacheSlot> cache_;

  LightEngineCacheStats stats;
};

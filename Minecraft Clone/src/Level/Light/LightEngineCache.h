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
    size_t set_block_light_ = 0;
    size_t set_sky_light_ = 0;
    size_t get_block_light_ = 0;
    size_t get_sky_light_ = 0;
    size_t get_block_ = 0;
    size_t get_chunk_ = 0;
    size_t check_chunk_ = 0;
    size_t try_cache_chunk_ = 0;
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
  Chunk* GetChunk(ChunkPos pos);

  void UpdateLight();
  bool CheckChunkHasLighting(ChunkPos pos);
  LightEngineCacheStats GetStats() const noexcept;

 private:
  void SetBlockCache(ChunkPos pos, Chunk* data);
  void SetBlockLightCache(ChunkPos pos, LightStorage* data);
  void SetSkyLightCache(ChunkPos pos, LightStorage* data);
  void ExpandDown();
  bool TryCacheChunk(ChunkPos pos);
  size_t CalculateCacheIndex(ChunkPos pos) const noexcept;

  static constexpr size_t kCacheSize = 3;

  GameContext& game_context_;
  WorldInterface& world_;

  ChunkPos center_chunk_;
  int y_top;
  int y_bot;

  LightEngineCacheStats stats;

  // first index is the top
  // Chunk, IsLightDirty
  std::vector<std::pair<Chunk*, bool>> chunk_cache_;
  std::vector<std::array<BlockID, kChunkSize3D>> block_data_;
  std::vector<LightStorage*> block_light_cache_;
  std::vector<LightStorage*> sky_light_cache_;
};

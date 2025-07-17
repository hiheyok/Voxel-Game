#include "Level/Light/LightEngineCache.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/LightStorage.h"
#include "Level/World/WorldInterface.h"

LightEngineCache::LightEngineCache(GameContext& game_context,
                                   WorldInterface& world)
    : game_context_{game_context}, world_{world} {}

void LightEngineCache::BuildCache(BlockPos center) {
  center_chunk_ = center.ToChunkPos();

  int cx = center_chunk_.x;
  int cy = center_chunk_.y +
           kCacheRadius;  // + kCacheRadius to start building the top part first
  int cz = center_chunk_.z;

  y_top = center_chunk_.y + kCacheRadius;
  y_bot = center_chunk_.y - kCacheRadius;

  cache_.resize(kCacheWidth * kCacheWidth * kCacheWidth);

  // Go down and level by level
  for (int y = cy; y > cy - kCacheWidth; --y) {
    for (int x = cx - kCacheRadius; x <= cx + kCacheRadius; ++x) {
      for (int z = cz - kCacheRadius; z <= cz + kCacheRadius; ++z) {
        ChunkPos pos{x, y, z};
        TryCacheChunk(pos);
      }
    }
  }
}

int LightEngineCache::GetBlockLight(BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);
  EnsureLoaded(chunk_pos);
  LightStorage* data = cache_[idx].GetBlock();
  assert(data != nullptr);
  return data->GetLighting(pos.GetLocalPos());
}

int LightEngineCache::GetSkyLight(BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);
  EnsureLoaded(chunk_pos);
  LightStorage* data = cache_[idx].GetSky();
  assert(data != nullptr);
  return data->GetLighting(pos.GetLocalPos());
}

// Setters and getter
BlockID LightEngineCache::GetBlock(BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  EnsureLoaded(chunk_pos);
  size_t idx = CalculateCacheIndex(chunk_pos);
  Chunk* data = cache_[idx].GetChunk();
  assert(data != nullptr);
  return data->GetBlockUnsafe(pos);
}

void LightEngineCache::SetBlockLight(BlockPos pos, int lvl) {
  assert(lvl >= 0 && lvl <= kMaxLightLevel);
  ChunkPos chunk_pos = pos.ToChunkPos();
  EnsureLoaded(chunk_pos);

  size_t idx = CalculateCacheIndex(chunk_pos);
  LightStorage* data = cache_[idx].GetBlock();
  assert(data != nullptr);
  data->EditLight(pos.GetLocalPos(), lvl);
  cache_[idx].SetDirty(true);
}
void LightEngineCache::SetSkyLight(BlockPos pos, int lvl) {
  assert(lvl >= 0 && lvl <= kMaxLightLevel);
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);

  EnsureLoaded(chunk_pos);
  LightStorage* data = cache_[idx].GetSky();
  assert(data != nullptr);
  cache_[idx].SetDirty(true);
  data->EditLight(pos, lvl);
}

bool LightEngineCache::CheckChunk(ChunkPos chunk_pos) {
  if (!EnsureLoaded(chunk_pos)) {
    return false;
  }
  size_t idx = CalculateCacheIndex(chunk_pos);
  return cache_[idx].GetChunk() != nullptr;
}

Chunk* LightEngineCache::GetChunk(ChunkPos chunk_pos) {
  size_t idx = CalculateCacheIndex(chunk_pos);
  EnsureLoaded(chunk_pos);
  Chunk* data = cache_[idx].GetChunk();
  assert(data != nullptr);
  return data;
}

void LightEngineCache::UpdateLight() {
  for (auto& cache : cache_) {
    if (cache.IsDirty()) {
      cache.GetChunk()->SetLightDirty();
    }
  }
}

bool LightEngineCache::CheckChunkHasLighting(ChunkPos pos) {
  size_t idx = CalculateCacheIndex(pos);
  Chunk* chunk = cache_[idx].GetChunk();
  if (chunk == nullptr && !CheckChunk(pos)) {
    return false;
  }
  chunk = cache_[idx].GetChunk();
  return chunk->IsLightUp();
}

LightEngineCache::LightEngineCacheStats LightEngineCache::GetStats()
    const noexcept {
  return stats;
}

// Helper function for caching
void LightEngineCache::SetBlockCache(ChunkPos pos, Chunk* data) {
  int idx = CalculateCacheIndex(pos);
  cache_[idx].SetChunk(data);
}
void LightEngineCache::SetBlockLightCache(ChunkPos pos, LightStorage* data) {
  int idx = CalculateCacheIndex(pos);
  cache_[idx].SetBlock(data);
}
void LightEngineCache::SetSkyLightCache(ChunkPos pos, LightStorage* data) {
  int idx = CalculateCacheIndex(pos);
  cache_[idx].SetSky(data);
}

void LightEngineCache::ExpandDown() {
  size_t curr_size = cache_.size();
  int curr_y = --y_bot;

  cache_.resize(curr_size + kCacheWidth * kCacheWidth);

  for (int x = center_chunk_.x - kCacheRadius;
       x <= center_chunk_.x + kCacheRadius; ++x) {
    for (int z = center_chunk_.z - kCacheRadius;
         z <= center_chunk_.z + kCacheRadius; ++z) {
      ChunkPos curr_pos{x, curr_y, z};
      TryCacheChunk(curr_pos);
    }
  }
}

bool LightEngineCache::TryCacheChunk(ChunkPos pos) {
  if (!world_.CheckChunk(pos)) {
    return false;
  }

  Chunk* chunk = world_.GetChunk(pos);
  LightStorage* sky_light = chunk->sky_light_.get();
  LightStorage* block_light = chunk->block_light_.get();

  SetSkyLightCache(pos, sky_light);
  SetBlockCache(pos, chunk);

  if (y_top - pos.y < kCacheWidth) {
    SetBlockLightCache(pos, block_light);
  }
  ++stats.try_cache_chunk_;
  return true;
}

size_t LightEngineCache::CalculateCacheIndex(ChunkPos pos) const noexcept {
  int x = (center_chunk_.x - pos.x + kCacheRadius);
  int y = (center_chunk_.y - pos.y + kCacheRadius);
  int z = (center_chunk_.z - pos.z + kCacheRadius);
  return y * kCacheWidth * kCacheWidth + x * kCacheWidth + z;
}

bool LightEngineCache::EnsureLoaded(ChunkPos pos) {
  assert(pos.y >= y_bot);
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].GetChunk() != nullptr;
}

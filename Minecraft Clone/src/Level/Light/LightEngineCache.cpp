#include "Level/Light/LightEngineCache.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/LightStorage.h"
#include "Level/World/WorldInterface.h"

LightEngineCache::LightEngineCache(GameContext& game_context,
                                   WorldInterface& world)
    : game_context_{game_context}, world_{world} {}

void LightEngineCache::BuildCache(BlockPos center) {
  cache_.resize(kCacheSlice * kCacheWidth);

  center_chunk_ = center.ToChunkPos();

  y_top = center_chunk_.y + kCacheRadius;
  y_bot = center_chunk_.y - kCacheRadius;

  // + 1 bc of Product iterator (< not <=)
  int x0 = center_chunk_.x - kCacheRadius;
  int x1 = center_chunk_.x + kCacheRadius + 1;
  int y0 = center_chunk_.y - kCacheRadius;
  int y1 = center_chunk_.y + kCacheRadius + 1;
  int z0 = center_chunk_.z - kCacheRadius;
  int z1 = center_chunk_.z + kCacheRadius + 1;

  for (auto [x, y, z] : Product<3>({{x0, x1}, {y0, y1}, {z0, z1}})) {
    ChunkPos pos{x, y, z};
    TryCacheChunk(pos);
  }
}

int LightEngineCache::GetBlockLight(BlockPos pos) {
  const ChunkPos chunk_pos = pos.ToChunkPos();
  const LightStorage* data = EnsureLoadedGetBlock(chunk_pos);
  assert(data != nullptr);
  return data->GetLighting(pos);
}

int LightEngineCache::GetSkyLight(BlockPos pos) {
  const ChunkPos chunk_pos = pos.ToChunkPos();
  const LightStorage* data = EnsureLoadedGetSky(chunk_pos);
  assert(data != nullptr);
  return data->GetLighting(pos);
}

// Setters and getter
BlockID LightEngineCache::GetBlock(BlockPos pos) {
  const ChunkPos chunk_pos = pos.ToChunkPos();
  const Chunk* data = EnsureLoadedGetChunk(chunk_pos);
  assert(data != nullptr);
  return data->GetBlockUnsafe(pos);
}

void LightEngineCache::SetBlockLight(BlockPos pos, int lvl) {
  assert(lvl >= 0 && lvl <= kMaxLightLevel);
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);
  LightStorage* data = EnsureLoadedGetBlock(chunk_pos);
  assert(data != nullptr);
  data->EditLight(pos.GetLocalPos(), lvl);
  cache_[idx].SetDirty(true);
}
void LightEngineCache::SetSkyLight(BlockPos pos, int lvl) {
  assert(lvl >= 0 && lvl <= kMaxLightLevel);
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);
  LightStorage* data = EnsureLoadedGetSky(chunk_pos);
  assert(data != nullptr);
  cache_[idx].SetDirty(true);
  data->EditLight(pos, lvl);
}

bool LightEngineCache::CheckChunk(ChunkPos chunk_pos) {
  const Chunk* chunk = EnsureLoadedGetChunk(chunk_pos);
  return chunk != nullptr;
}

Chunk* LightEngineCache::GetChunk(ChunkPos chunk_pos) {
  Chunk* data = EnsureLoadedGetChunk(chunk_pos);
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
  size_t idx = CalculateCacheIndex(pos);
  cache_[idx].SetChunk(data);
}
void LightEngineCache::SetBlockLightCache(ChunkPos pos, LightStorage* data) {
  size_t idx = CalculateCacheIndex(pos);
  cache_[idx].SetBlock(data);
}
void LightEngineCache::SetSkyLightCache(ChunkPos pos, LightStorage* data) {
  size_t idx = CalculateCacheIndex(pos);
  cache_[idx].SetSky(data);
}

void LightEngineCache::ExpandDown() {
  size_t curr_size = cache_.size();
  int curr_y = --y_bot;

  cache_.resize(curr_size + kCacheSlice);

  int x0 = center_chunk_.x - kCacheRadius;
  int x1 = center_chunk_.x + kCacheRadius + 1;
  int z0 = center_chunk_.z - kCacheRadius;
  int z1 = center_chunk_.z + kCacheRadius + 1;

  for (auto [x, z] : Product<2>({{x0, x1}, {z0, z1}})) {
    ChunkPos curr_pos{x, curr_y, z};
    TryCacheChunk(curr_pos);
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
  return true;
}

size_t LightEngineCache::CalculateCacheIndex(ChunkPos pos) const noexcept {
  const int x = center_chunk_.x - pos.x + kCacheRadius;
  const int y = center_chunk_.y - pos.y + kCacheRadius;
  const int z = center_chunk_.z - pos.z + kCacheRadius;
  return y * kCacheSlice + x * kCacheWidth + z;
}

bool LightEngineCache::EnsureLoaded(ChunkPos pos) {
  assert(pos.y >= y_bot);
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].GetChunk() != nullptr;
}

LightStorage* LightEngineCache::EnsureLoadedGetSky(ChunkPos pos) {
  assert(pos.y >= y_bot);
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].GetSky();
}
LightStorage* LightEngineCache::EnsureLoadedGetBlock(ChunkPos pos) {
  assert(pos.y >= y_bot);
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].GetBlock();
}
Chunk* LightEngineCache::EnsureLoadedGetChunk(ChunkPos pos) {
  assert(pos.y >= y_bot);
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].GetChunk();
}

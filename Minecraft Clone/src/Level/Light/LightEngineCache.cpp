#include "Level/Light/LightEngineCache.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/LightStorage.h"
#include "Level/World/WorldInterface.h"
#include "Utils/Assert.h"

LightEngineCache::LightEngineCache(GameContext& context, WorldInterface& world)
    : context_{context}, world_{world} {}

void LightEngineCache::BuildCache(BlockPos center) {
  cache_.resize(kCacheSlice * kCacheWidth);
  dirty_.resize(kCacheSlice * kCacheWidth);

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
  const LightStorage* data = EnsureLoadedGetBlock(pos);
  GAME_ASSERT(data != nullptr, "Block light data is null");
  return data->GetLighting(pos);
}

int LightEngineCache::GetSkyLight(BlockPos pos) {
  const LightStorage* data = EnsureLoadedGetSky(pos);
  GAME_ASSERT(data != nullptr, "Sky light data is null");
  return data->GetLighting(pos);
}

// Setters and getter
BlockID LightEngineCache::GetBlock(BlockPos pos) {
  const Chunk* data = EnsureLoadedGetChunk(pos);
  GAME_ASSERT(data != nullptr, "Chunk data is null");
  return data->GetBlockUnsafe(pos.GetLocalPos());
}

void LightEngineCache::SetBlockLight(BlockPos pos, int lvl) {
  GAME_ASSERT(lvl >= 0 && lvl <= kMaxLightLevel, "Light level out of range");
  size_t idx = CalculateCacheIndex(pos);
  LightStorage* data = EnsureLoadedGetBlock(pos);
  GAME_ASSERT(data != nullptr, "Block light data is null");
  data->EditLight(pos, lvl);
  dirty_[idx] = true;
}
void LightEngineCache::SetSkyLight(BlockPos pos, int lvl) {
  GAME_ASSERT(lvl >= 0 && lvl <= kMaxLightLevel, "Light level out of range");
  size_t idx = CalculateCacheIndex(pos);
  LightStorage* data = EnsureLoadedGetSky(pos);
  GAME_ASSERT(data != nullptr, "Sky light data is null");
  dirty_[idx] = true;
  data->EditLight(pos, lvl);
}

bool LightEngineCache::CheckChunk(ChunkPos chunk_pos) {
  const Chunk* chunk = EnsureLoadedGetChunk(chunk_pos);
  return chunk != nullptr;
}

bool LightEngineCache::CheckChunk(BlockPos pos) {
  const Chunk* chunk = EnsureLoadedGetChunk(pos);
  return chunk != nullptr;
}

Chunk* LightEngineCache::GetChunk(ChunkPos chunk_pos) {
  Chunk* data = EnsureLoadedGetChunk(chunk_pos);
  GAME_ASSERT(data != nullptr, "Chunk data is null");
  return data;
}

Chunk* LightEngineCache::GetChunk(BlockPos pos) {
  Chunk* data = EnsureLoadedGetChunk(pos);
  GAME_ASSERT(data != nullptr, "Chunk data is null");
  return data;
}

void LightEngineCache::UpdateLight() {
  for (size_t i = 0; i < cache_.size(); ++i) {
    if (dirty_[i] && cache_[i].chunk_) {
      cache_[i].chunk_->SetLightDirty();
    }
  }
}

bool LightEngineCache::CheckChunkHasLighting(ChunkPos pos) {
  size_t idx = CalculateCacheIndex(pos);
  Chunk* chunk = cache_[idx].chunk_;
  if (chunk == nullptr && !CheckChunk(pos)) {
    return false;
  }
  chunk = cache_[idx].chunk_;
  return chunk->IsLightUp();
}

LightEngineCache::LightEngineCacheStats LightEngineCache::GetStats()
    const noexcept {
  return stats;
}

// Helper function for caching
void LightEngineCache::SetBlockCache(ChunkPos pos, Chunk* data) noexcept {
  size_t idx = CalculateCacheIndex(pos);
  cache_[idx].chunk_ = data;
}
void LightEngineCache::SetBlockLightCache(ChunkPos pos,
                                          LightStorage* data) noexcept {
  size_t idx = CalculateCacheIndex(pos);
  cache_[idx].block_ = data;
}
void LightEngineCache::SetSkyLightCache(ChunkPos pos,
                                        LightStorage* data) noexcept {
  size_t idx = CalculateCacheIndex(pos);
  cache_[idx].sky_ = data;
}

void LightEngineCache::ExpandDown() {
  size_t curr_size = cache_.size();
  int curr_y = --y_bot;

  cache_.resize(curr_size + kCacheSlice);
  dirty_.resize(curr_size + kCacheSlice);

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
  if (!world_.CheckChunk(pos)) [[unlikely]] {
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

size_t LightEngineCache::CalculateCacheIndex(BlockPos pos) const noexcept {
  const int x = center_chunk_.x - (pos.x >> kChunkDimLog2) + kCacheRadius;
  const int y = center_chunk_.y - (pos.y >> kChunkDimLog2) + kCacheRadius;
  const int z = center_chunk_.z - (pos.z >> kChunkDimLog2) + kCacheRadius;
  return y * kCacheSlice + x * kCacheWidth + z;
}

bool LightEngineCache::EnsureLoaded(ChunkPos pos) {
  GAME_ASSERT(pos.y >= y_bot, "Position Y below cache bottom");
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].chunk_ != nullptr;
}

LightStorage* LightEngineCache::EnsureLoadedGetSky(ChunkPos pos) {
  GAME_ASSERT(pos.y >= y_bot, "Position Y below cache bottom");
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].sky_;
}
LightStorage* LightEngineCache::EnsureLoadedGetSky(BlockPos pos) {
  GAME_ASSERT((pos.y >> kChunkDimLog2) >= y_bot,
              "Position Y below cache bottom");
  if ((pos.y >> kChunkDimLog2) == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].sky_;
}
LightStorage* LightEngineCache::EnsureLoadedGetBlock(ChunkPos pos) {
  GAME_ASSERT(pos.y >= y_bot, "Position Y below cache bottom");
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].block_;
}
LightStorage* LightEngineCache::EnsureLoadedGetBlock(BlockPos pos) {
  GAME_ASSERT((pos.y >> kChunkDimLog2) >= y_bot,
              "Position Y below cache bottom");
  if ((pos.y >> kChunkDimLog2) == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].block_;
}
Chunk* LightEngineCache::EnsureLoadedGetChunk(ChunkPos pos) {
  GAME_ASSERT(pos.y >= y_bot, "Position Y below cache bottom");
  if (pos.y == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].chunk_;
}

Chunk* LightEngineCache::EnsureLoadedGetChunk(BlockPos pos) {
  GAME_ASSERT((pos.y >> kChunkDimLog2) >= y_bot,
              "Position Y below cache bottom");
  if ((pos.y >> kChunkDimLog2) == y_bot) [[unlikely]] {
    ExpandDown();
  }
  const size_t idx = CalculateCacheIndex(pos);
  return cache_[idx].chunk_;
}

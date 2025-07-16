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
  int cy = center_chunk_.y + 1;  // + 1 to start building the top part first
  int cz = center_chunk_.z;

  y_top = center_chunk_.y + 1;
  y_bot = center_chunk_.y - 1;

  chunk_cache_.resize(3 * 3 * 3);
  block_light_cache_.resize(3 * 3 * 3);
  sky_light_cache_.resize(3 * 3 * 3);
  block_data_.resize(3 * 3 * 3);

  // Go down and level by level
  for (int y = cy; y > cy - 3; --y) {
    for (int x = cx - 1; x <= cx + 1; ++x) {
      for (int z = cz - 1; z <= cz + 1; ++z) {
        ChunkPos pos{x, y, z};
        TryCacheChunk(pos);
      }
    }
  }
}

int LightEngineCache::GetBlockLight(BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);

  LightStorage* data = block_light_cache_[idx];
  if (data == nullptr) {
    bool res = TryCacheChunk(chunk_pos);
    assert(res);
    data = block_light_cache_[idx];
  }
  ++stats.get_block_light_;
  return data->GetLighting(pos.GetLocalPos());
}
int LightEngineCache::GetSkyLight(BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);

  assert(chunk_pos.y >= y_bot - 1);
  if (chunk_pos.y == y_bot - 1) {
    ExpandDown();
  }

  LightStorage* data = sky_light_cache_[idx];
  if (data == nullptr) {
    bool res = TryCacheChunk(chunk_pos);
    assert(res);
    data = sky_light_cache_[idx];
  }

  ++stats.get_sky_light_;
  return data->GetLighting(pos.GetLocalPos());
}

// Setters and getter
BlockID LightEngineCache::GetBlock(BlockPos pos) {
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);

  Chunk* data = chunk_cache_[idx].first;
  if (data == nullptr) {
    TryCacheChunk(chunk_pos);
    data = chunk_cache_[idx].first;
    assert(data != nullptr);
  }

  ++stats.get_block_;
  return block_data_[idx][pos.GetIndex()];
}

void LightEngineCache::SetBlockLight(BlockPos pos, int lvl) {
  assert(lvl >= 0 && lvl <= kMaxLightLevel);
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);

  LightStorage* data = block_light_cache_[idx];
  if (data == nullptr) {
    bool res = TryCacheChunk(chunk_pos);
    assert(res);
    data = block_light_cache_[idx];
  }

  if (data->GetLighting(pos.GetLocalPos()) != lvl) {
    data->EditLight(pos.GetLocalPos(), lvl);
    chunk_cache_[idx].second = true;
  }
  ++stats.set_block_light_;
}
void LightEngineCache::SetSkyLight(BlockPos pos, int lvl) {
  assert(lvl >= 0 && lvl <= kMaxLightLevel);
  ChunkPos chunk_pos = pos.ToChunkPos();
  size_t idx = CalculateCacheIndex(chunk_pos);

  // Make sure it doesn't look too far ahead
  assert(chunk_pos.y >= y_bot - 1);
  if (chunk_pos.y == y_bot - 1) {
    ExpandDown();
  }

  LightStorage* data = sky_light_cache_[idx];
  if (data == nullptr) {
    bool res = TryCacheChunk(chunk_pos);
    assert(res);
    data = sky_light_cache_[idx];
  }
  if (data->GetLighting(pos.GetLocalPos()) != lvl) {
    data->EditLight(pos.GetLocalPos(), lvl);
    chunk_cache_[idx].second = true;
  }
  ++stats.set_sky_light_;
}

bool LightEngineCache::CheckChunk(ChunkPos chunk_pos) {
  size_t idx = CalculateCacheIndex(chunk_pos);

  // Make sure it doesn't look too far ahead
  assert(chunk_pos.y >= y_bot - 1);
  if (chunk_pos.y == y_bot - 1) {
    ExpandDown();
  }

  Chunk* data = chunk_cache_[idx].first;
  if (data == nullptr) [[unlikely]] {
    TryCacheChunk(chunk_pos);
    data = chunk_cache_[idx].first;
  }
  ++stats.check_chunk_;
  return data != nullptr;
}

Chunk* LightEngineCache::GetChunk(ChunkPos chunk_pos) {
  size_t idx = CalculateCacheIndex(chunk_pos);

  // Make sure it doesn't look too far ahead
  assert(chunk_pos.y >= y_bot - 1);
  if (chunk_pos.y == y_bot - 1) {
    ExpandDown();
  }

  Chunk* data = chunk_cache_[idx].first;
  if (data == nullptr) {
    bool res = TryCacheChunk(chunk_pos);
    assert(res);
    data = chunk_cache_[idx].first;
  }

  ++stats.get_chunk_;
  return data;
}

void LightEngineCache::UpdateLight() {
  for (auto& [chunk, is_dirty] : chunk_cache_) {
    if (is_dirty) {
      chunk->SetLightDirty();
    }
  }
}

bool LightEngineCache::CheckChunkHasLighting(ChunkPos pos) {
  size_t idx = CalculateCacheIndex(pos);
  Chunk* chunk = chunk_cache_[idx].first;
  if (!CheckChunk(pos)) {
    return false;
  }
  chunk = chunk_cache_[idx].first;
  return chunk->IsLightUp();
}

LightEngineCache::LightEngineCacheStats LightEngineCache::GetStats()
    const noexcept {
  return stats;
}

// Helper function for caching
void LightEngineCache::SetBlockCache(ChunkPos pos, Chunk* data) {
  int idx = CalculateCacheIndex(pos);
  chunk_cache_[idx].first = data;
  block_data_[idx] = data->GetPalette().UnpackAll();
}
void LightEngineCache::SetBlockLightCache(ChunkPos pos, LightStorage* data) {
  int idx = CalculateCacheIndex(pos);
  block_light_cache_[idx] = data;
}
void LightEngineCache::SetSkyLightCache(ChunkPos pos, LightStorage* data) {
  int idx = CalculateCacheIndex(pos);
  sky_light_cache_[idx] = data;
}

void LightEngineCache::ExpandDown() {
  size_t curr_size = chunk_cache_.size();
  int curr_y = --y_bot;

  chunk_cache_.resize(curr_size + 3 * 3);
  sky_light_cache_.resize(curr_size + 3 * 3);
  block_data_.resize(curr_size + 3 * 3);

  for (int x = center_chunk_.x - 1; x <= center_chunk_.x + 1; ++x) {
    for (int z = center_chunk_.z - 1; z <= center_chunk_.z + 1; ++z) {
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

  if (y_top - pos.y < 3) {
    SetBlockLightCache(pos, block_light);
  }
  ++stats.try_cache_chunk_;
  return true;
}

size_t LightEngineCache::CalculateCacheIndex(ChunkPos pos) const noexcept {
  return (center_chunk_.y - pos.y + 1) * kCacheSize * kCacheSize +
         (center_chunk_.x - pos.x + 1) * kCacheSize +
         (center_chunk_.z - pos.z + 1);
}

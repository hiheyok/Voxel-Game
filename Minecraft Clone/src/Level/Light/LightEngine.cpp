#include "Level/Light/LightEngine.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Position/Direction.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/Light/LightStorage.h"
#include "Level/World/WorldInterface.h"
#include "Utils/Assert.h"

using std::vector;

template <EngineType kEngineType>
LightEngine<kEngineType>::LightEngine(GameContext& context,
                                      WorldInterface& world)
    : context_{context},
      world_{world},
      properties_{context.blocks_->GetBlockPropertyList()},
      light_cache_{nullptr} {}

template <EngineType kEngineType>
LightEngine<kEngineType>::~LightEngine() = default;

template <EngineType kEngineType>
void LightEngine<kEngineType>::SetCache(LightEngineCache* cache) {
  GAME_ASSERT(cache != nullptr, "Cache is null");
  light_cache_ = cache;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::Propagate(const ChunkLightTask& chunk_task) {
  GAME_ASSERT(light_cache_ != nullptr, "Light cache is null");
  PropagateChanges(chunk_task);
  decrease_queue_.Clear();
  increase_queue_.Clear();
  light_cache_ = nullptr;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::PropagateIncrease() {
  InternalTask next_task;
  // to prevent pointer chasing
  const auto& block_properties = context_.blocks_->GetBlockPropertyList();

  ChunkPos src_chunk_pos = light_cache_->center_chunk_;
  size_t src_cache_idx = light_cache_->CalculateCacheIndex(src_chunk_pos);
  auto* src_sky_light = light_cache_->GetSlot(src_cache_idx).sky_;
  auto* src_block_light = light_cache_->GetSlot(src_cache_idx).block_;
  auto* src_chunk = light_cache_->GetSlot(src_cache_idx).chunk_;

  size_t curr_cache_idx = src_cache_idx;
  auto* curr_sky_light = src_sky_light;
  auto* curr_block_light = src_block_light;
  auto* curr_chunk = src_chunk;

  vector<InternalTask> tasks = increase_queue_.PopBucket();

  while (tasks.size() > 0) {
    const int propagation_lvl = tasks[0].light_lvl_;

    for (const auto& task : tasks) {
      const BlockPos block_pos = task.block_pos_;
      const int direction = task.direction_;

      ChunkPos curr_chunk_pos = block_pos.ToChunkPos();

      // Cache source chunk info - reuse for same-chunk neighbors
      if (curr_chunk_pos != src_chunk_pos) [[unlikely]] {
        src_cache_idx = light_cache_->CalculateCacheIndex(curr_chunk_pos);
        auto& slot = light_cache_->GetSlot(src_cache_idx);
        src_sky_light = slot.sky_;
        src_block_light = slot.block_;
        src_chunk = slot.chunk_;

        curr_cache_idx = src_cache_idx;
        curr_sky_light = src_sky_light;
        curr_block_light = src_block_light;
        curr_chunk = src_chunk;

        src_chunk_pos = curr_chunk_pos;
      }

      // Light level already changed from somewhere else
      if (task.recheck_light_) {
        int current_lvl;
        if constexpr (kEngineType == EngineType::kBlockLight) {
          current_lvl = curr_block_light->GetLighting(block_pos);
        } else {
          current_lvl = curr_sky_light->GetLighting(block_pos);
        }
        if (propagation_lvl != current_lvl) {
          continue;
        }
      }

      // Static reference to avoid any template instantiation overhead
      static constexpr auto& kDirs = Directions<BlockPos>::kDirections;
      for (const auto& propagation : kDirs) {
        if constexpr (kEngineType == EngineType::kSkyLight) {
          if (propagation == kUpDirection) {
            continue;
          }
        }

        if (direction != kAllDirections &&
            direction == propagation.GetOppositeDirection()) {
          continue;
        }

        BlockPos offset_pos = block_pos + propagation;

        // Check if neighbor is in a different chunk
        ChunkPos offset_chunk_pos = offset_pos.ToChunkPos();
        if (offset_chunk_pos != src_chunk_pos) [[unlikely]] {
          // Different chunk - need to check and get new slot
          if (!light_cache_->CheckChunk(offset_pos)) [[unlikely]] {
            continue;
          }
          curr_cache_idx = light_cache_->CalculateCacheIndex(offset_pos);
          auto& slot = light_cache_->GetSlot(curr_cache_idx);
          curr_sky_light = slot.sky_;
          curr_block_light = slot.block_;
          curr_chunk = slot.chunk_;
        } else {
          // Same chunk as source - reset to cached source slot
          curr_cache_idx = src_cache_idx;
          curr_sky_light = src_sky_light;
          curr_block_light = src_block_light;
          curr_chunk = src_chunk;
        }

        // Get light level directly from slot
        int curr_lvl;
        if constexpr (kEngineType == EngineType::kBlockLight) {
          curr_lvl = curr_block_light->GetLighting(offset_pos);
        } else {
          curr_lvl = curr_sky_light->GetLighting(offset_pos);
        }

        if (curr_lvl >= propagation_lvl - 1) {
          continue;
        }

        // Get block directly from slot
        BlockID offset_block =
            curr_chunk->GetBlockUnsafe(offset_pos.GetLocalPos());
        int block_opacity = block_properties[offset_block].opacity_;
        int target_lvl = propagation_lvl - std::max(1, block_opacity);
        target_lvl = std::max(0, target_lvl);
        // Area is already brightly lit
        if (target_lvl <= curr_lvl) {
          continue;
        }

        // Set light level directly using slot
        if constexpr (kEngineType == EngineType::kBlockLight) {
          curr_block_light->EditLight(offset_pos, target_lvl);
        } else {
          curr_sky_light->EditLight(offset_pos, target_lvl);
        }
        light_cache_->MarkDirty(curr_cache_idx);

        // If target lvl is <= 1, then its not going to be able to spread and
        // value is already set
        if (target_lvl > 1) [[likely]] {
          next_task.block_pos_ = offset_pos;
          next_task.light_lvl_ = static_cast<uint8_t>(target_lvl);
          next_task.direction_ = propagation;
          increase_queue_.Push(next_task);
        }
      }
    }
    tasks = increase_queue_.PopBucket();
  }
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::PropagateDecrease() {
  InternalTask next_task;
  next_task.recheck_light_ = true;

  vector<InternalTask> tasks = decrease_queue_.PopBucket();

  while (tasks.size() > 0) {
    const int propagation_lvl = tasks[0].light_lvl_;

    for (const auto& task : tasks) {
      const BlockPos block_pos = task.block_pos_;

      static constexpr auto& kDirs = Directions<BlockPos>::kDirections;
      for (const auto& propagation : kDirs) {
        BlockPos offset_pos = block_pos + propagation;
        // Chunk doesnt exist
        if (!light_cache_->CheckChunk(offset_pos)) [[unlikely]] {
          continue;
        }

        int curr_lvl = GetLightLvl(offset_pos);

        // Area is already dark
        if (curr_lvl == 0) {
          continue;
        }

        // If neighbor is dimmer, it might have been lit by the removed light.
        // Remove its light and add it to the decrease queue to propagate
        // darkness.

        if (curr_lvl < propagation_lvl) {
          SetLightLvl(offset_pos, 0);
          next_task.block_pos_ = offset_pos;
          next_task.light_lvl_ = static_cast<uint8_t>(curr_lvl);
          next_task.direction_ = propagation;
          decrease_queue_.Push(next_task);
        } else {
          // If neighbor is brighter or equal, it is an independent light
          // source. Add it to the increase queue to re-light the new darkness.
          next_task.block_pos_ = offset_pos;
          next_task.light_lvl_ = static_cast<uint8_t>(curr_lvl);
          next_task.direction_ = propagation.GetOppositeDirection();
          increase_queue_.Push(next_task);
        }
      }
    }
    tasks = decrease_queue_.PopBucket();
  }

  PropagateIncrease();
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::SetLightLvl(BlockPos block_pos, int light_lvl) {
  if constexpr (kEngineType == EngineType::kBlockLight) {
    light_cache_->SetBlockLight(block_pos, light_lvl);
  } else {
    light_cache_->SetSkyLight(block_pos, light_lvl);
  }
}

template <EngineType kEngineType>
int LightEngine<kEngineType>::GetLightLvl(BlockPos block_pos) {
  int lvl = 0;
  if constexpr (kEngineType == EngineType::kBlockLight) {
    lvl = light_cache_->GetBlockLight(block_pos);
  } else {
    lvl = light_cache_->GetSkyLight(block_pos);
  }
  return lvl;
}

template <EngineType kEngineType>
BlockID LightEngine<kEngineType>::GetBlock(BlockPos pos) {
  return light_cache_->GetBlock(pos);
}

template <EngineType kEngineType>
bool LightEngine<kEngineType>::CheckChunk(ChunkPos pos) {
  return light_cache_->CheckChunk(pos);
}

template <EngineType kEngineType>
Chunk* LightEngine<kEngineType>::GetChunk(ChunkPos pos) {
  return light_cache_->GetChunk(pos);
}

template class LightEngine<EngineType::kBlockLight>;
template class LightEngine<EngineType::kSkyLight>;

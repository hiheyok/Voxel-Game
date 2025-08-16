#include "Level/Light/LightEngine.h"

#include <algorithm>
#include <cassert>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/World/WorldInterface.h"

template <EngineType kEngineType>
void LightEngine<kEngineType>::InternalTask::SetBlockPos(
    BlockPos pos) noexcept {
  block_pos_ = pos;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::InternalTask::SetDirection(
    int direction) noexcept {
  direction_ = direction;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::InternalTask::SetLightLevel(int lvl) noexcept {
  assert(lvl >= 0 && lvl < 16);
  light_lvl_ = static_cast<uint8_t>(lvl);
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::InternalTask::SetRecheckLight(bool b) noexcept {
  recheck_light_ = b;
}

template <EngineType kEngineType>
BlockPos LightEngine<kEngineType>::InternalTask::GetBlockPos() const noexcept {
  return block_pos_;
}

template <EngineType kEngineType>
int LightEngine<kEngineType>::InternalTask::GetDirection() const noexcept {
  return direction_;
}

template <EngineType kEngineType>
int LightEngine<kEngineType>::InternalTask::GetLightLevel() const noexcept {
  return light_lvl_;
}

template <EngineType kEngineType>
bool LightEngine<kEngineType>::InternalTask::GetRecheckLight() const noexcept {
  return recheck_light_;
}

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
  assert(cache != nullptr);
  light_cache_ = cache;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::Propagate(const ChunkLightTask& chunk_task) {
  assert(light_cache_ != nullptr);
  PropagateChanges(chunk_task);
  ResetDecreaseQueue();
  ResetIncreaseQueue();
  light_cache_ = nullptr;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::PropagateIncrease() {
  InternalTask task;
  InternalTask next_task;
  // to prevent pointer chasing
  const std::vector<BlockProperties>& block_properties =
      context_.blocks_->GetBlockPropertyList();

  BlockPos block_pos;
  int direction;
  int propagation_lvl;

  while (TryDequeueIncrease(task)) {
    block_pos = task.GetBlockPos();
    direction = task.GetDirection();
    propagation_lvl = task.GetLightLevel();

    // Light level already changed from somewhere else
    if (task.GetRecheckLight() && propagation_lvl != GetLightLvl(block_pos)) {
      continue;
    }

    for (auto propagation : Directions<BlockPos>()) {
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

      // Light level is already at the expected level or chunk doesn't exist

      if (!light_cache_->CheckChunk(offset_pos)) [[unlikely]] {
        continue;
      }

      int curr_lvl = GetLightLvl(offset_pos);

      if (curr_lvl >= propagation_lvl - 1) {
        continue;
      }

      BlockID offset_block = GetBlock(offset_pos);
      int block_opacity = block_properties[offset_block].opacity_;
      int target_lvl = propagation_lvl - std::max(1, block_opacity);
      target_lvl = std::max(0, target_lvl);
      // Area is already brightly lit
      if (target_lvl <= curr_lvl) {
        continue;
      }

      SetLightLvl(offset_pos, target_lvl);

      // If target lvl is <= 1, then its not going to be able to spread and
      // value is already set
      if (target_lvl > 1) [[likely]] {
        next_task.SetBlockPos(offset_pos);
        next_task.SetLightLevel(target_lvl);
        next_task.SetDirection(propagation);
        EnqueueIncrease(next_task);
      }
    }
  }
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::PropagateDecrease() {
  InternalTask task;
  InternalTask next_task;
  next_task.SetRecheckLight(true);

  while (TryDequeueDecrease(task)) {
    BlockPos block_pos = task.GetBlockPos();
    int propagation_lvl = task.GetLightLevel();

    for (auto propagation : Directions<BlockPos>()) {
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
        next_task.SetBlockPos(offset_pos);
        next_task.SetLightLevel(curr_lvl);
        next_task.SetDirection(propagation);
        EnqueueDecrease(next_task);
      } else {
        // If neighbor is brighter or equal, it is an independent light source.
        // Add it to the increase queue to re-light the new darkness.
        next_task.SetBlockPos(offset_pos);
        next_task.SetLightLevel(curr_lvl);
        next_task.SetDirection(propagation.GetOppositeDirection());
        EnqueueIncrease(next_task);
      }
    }
  }

  PropagateIncrease();
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::EnqueueIncrease(InternalTask task) {
  if (enqueue_increase_pos_ >= increase_queue_.size()) [[unlikely]] {
    EnlargeIncreaseQueue();
  }
  increase_queue_[enqueue_increase_pos_++] = task;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::EnqueueDecrease(InternalTask task) {
  if (enqueue_decrease_pos_ >= decrease_queue_.size()) [[unlikely]] {
    EnlargeDecreaseQueue();
  }
  decrease_queue_[enqueue_decrease_pos_++] = task;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::EnlargeIncreaseQueue() {
  increase_queue_.resize(increase_queue_.size() + kQueueSizeIncrement);
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::EnlargeDecreaseQueue() {
  decrease_queue_.resize(decrease_queue_.size() + kQueueSizeIncrement);
}

template <EngineType kEngineType>
LightEngine<kEngineType>::InternalTask
LightEngine<kEngineType>::DequeueIncrease() noexcept {
  assert(dequeue_increase_pos_ < enqueue_increase_pos_);
  return increase_queue_[dequeue_increase_pos_++];
}

template <EngineType kEngineType>
LightEngine<kEngineType>::InternalTask
LightEngine<kEngineType>::DequeueDecrease() noexcept {
  assert(dequeue_decrease_pos_ < enqueue_decrease_pos_);
  return decrease_queue_[dequeue_decrease_pos_++];
}

template <EngineType kEngineType>
bool LightEngine<kEngineType>::IsIncreaseEmpty() const noexcept {
  return enqueue_increase_pos_ == dequeue_increase_pos_;
}

template <EngineType kEngineType>
bool LightEngine<kEngineType>::IsDecreaseEmpty() const noexcept {
  return enqueue_decrease_pos_ == dequeue_decrease_pos_;
}

template <EngineType kEngineType>
bool LightEngine<kEngineType>::TryDequeueIncrease(InternalTask& task) noexcept {
  if (IsIncreaseEmpty()) [[unlikely]] {
    return false;
  }
  task = DequeueIncrease();
  return true;
}

template <EngineType kEngineType>
bool LightEngine<kEngineType>::TryDequeueDecrease(InternalTask& task) noexcept {
  if (IsDecreaseEmpty()) [[unlikely]] {
    return false;
  }
  task = DequeueDecrease();
  return true;
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::ResetIncreaseQueue() {
  enqueue_increase_pos_ = 0;
  dequeue_increase_pos_ = 0;
  increase_queue_.resize(kQueueSizeIncrement);
}

template <EngineType kEngineType>
void LightEngine<kEngineType>::ResetDecreaseQueue() {
  enqueue_decrease_pos_ = 0;
  dequeue_decrease_pos_ = 0;
  decrease_queue_.resize(kQueueSizeIncrement);
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

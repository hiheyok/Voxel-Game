#include "Level/Light/LightEngine.h"

#include <algorithm>
#include <cassert>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/World/WorldInterface.h"

void LightEngine::InternalTask::SetBlockPos(BlockPos pos) noexcept {
  block_pos_ = pos;
}

void LightEngine::InternalTask::SetDirection(int direction) noexcept {
  direction_ = direction;
}

void LightEngine::InternalTask::SetLightLevel(int lvl) noexcept {
  assert(lvl >= 0 && lvl < 16);
  light_lvl_ = static_cast<uint8_t>(lvl);
}

BlockPos LightEngine::InternalTask::GetBlockPos() const noexcept {
  return block_pos_;
}

int LightEngine::InternalTask::GetDirection() const noexcept {
  return direction_;
}

int LightEngine::InternalTask::GetLightLevel() const noexcept {
  return light_lvl_;
}

LightEngine::LightEngine(GameContext& game_context, WorldInterface& world)
    : game_context_{game_context}, world_{world}, light_cache_{nullptr} {}
LightEngine::~LightEngine() = default;

void LightEngine::SetCache(LightEngineCache* cache) {
  assert(cache != nullptr);
  light_cache_ = cache;
}

void LightEngine::Propagate(const ChunkLightTask& chunk_task) {
  assert(light_cache_ != nullptr);
  PropagateChanges(chunk_task);
  ResetDecreaseQueue();
  ResetIncreaseQueue();
  light_cache_ = nullptr;
}

void LightEngine::PropagateIncrease() {
  InternalTask task;
  while (TryDequeueIncrease(task)) {
    BlockPos block_pos = task.GetBlockPos();
    int direction = task.GetDirection();
    int propagation_lvl = task.GetLightLevel();
    int light_lvl = GetLightLvl(block_pos);

    // Light level already changed from somewhere else
    if (propagation_lvl != light_lvl) {
      [[likely]] continue;
    }

    for (auto propagation : Directions<BlockPos>()) {
      if (direction != kAllDirections &&
          direction == propagation.GetOppositeDirection()) {
        continue;
      }

      BlockPos offset_pos = block_pos + propagation;

      // Light level is already at the expected level or chunk doesn't exist
      if (!CheckChunk(offset_pos.ToChunkPos())) [[unlikely]] {
        continue;
      }

      int curr_lvl = GetLightLvl(offset_pos);

      if (curr_lvl >= propagation_lvl - 1) {
        [[unlikely]] continue;
      }

      BlockID offset_block = GetBlock(offset_pos);
      int block_opacity =
          game_context_.blocks_->GetBlockProperties(offset_block).opacity_;
      int target_lvl = propagation_lvl - std::max(1, block_opacity);
      target_lvl = std::max(0, target_lvl);
      // Area is already brightly lit
      if (target_lvl <= curr_lvl) {
        continue;
      }

      SetLightLvl(offset_pos, target_lvl);

      // If target lvl is <= 1, then its not going to be able to spread and
      // value is already set
      if (target_lvl > 1) {
        InternalTask next_task;
        next_task.SetBlockPos(offset_pos);
        next_task.SetLightLevel(target_lvl);
        next_task.SetDirection(propagation);
        EnqueueIncrease(next_task);
      }
    }
  }
}

void LightEngine::PropagateDecrease() {
  InternalTask task;

  while (TryDequeueDecrease(task)) {
    BlockPos block_pos = task.GetBlockPos();
    int propagation_lvl = task.GetLightLevel();

    for (auto propagation : Directions<BlockPos>()) {
      BlockPos offset_pos = block_pos + propagation;
      // Chunk doesnt exist
      if (!CheckChunk(offset_pos.ToChunkPos())) {
        [[unlikely]] continue;
      }

      int curr_lvl = GetLightLvl(offset_pos);

      // Area is already dark
      if (curr_lvl == 0) {
        [[unlikely]] continue;
      }

      // If neighbor is dimmer, it might have been lit by the removed light.
      // Remove its light and add it to the decrease queue to propagate
      // darkness.
      if (curr_lvl < propagation_lvl) {
        SetLightLvl(offset_pos, 0);
        InternalTask next_task;
        next_task.SetBlockPos(offset_pos);
        next_task.SetLightLevel(curr_lvl);
        next_task.SetDirection(propagation);
        EnqueueDecrease(next_task);
      } else {
        // If neighbor is brighter or equal, it is an independent light source.
        // Add it to the increase queue to re-light the new darkness.
        InternalTask next_task;
        next_task.SetBlockPos(offset_pos);
        next_task.SetLightLevel(curr_lvl);
        next_task.SetDirection(propagation.GetOppositeDirection());
        EnqueueIncrease(next_task);
      }
    }
  }

  PropagateIncrease();
}

void LightEngine::EnqueueIncrease(InternalTask task) {
  if (enqueue_increase_pos_ >= increase_queue_.size()) {
    EnlargeIncreaseQueue();
  }
  increase_queue_[enqueue_increase_pos_++] = task;
}

void LightEngine::EnqueueDecrease(InternalTask task) {
  if (enqueue_decrease_pos_ >= decrease_queue_.size()) {
    EnlargeDecreaseQueue();
  }
  decrease_queue_[enqueue_decrease_pos_++] = task;
}

void LightEngine::EnlargeIncreaseQueue() {
  increase_queue_.resize(increase_queue_.size() + kQueueSizeIncrement);
}

void LightEngine::EnlargeDecreaseQueue() {
  decrease_queue_.resize(decrease_queue_.size() + kQueueSizeIncrement);
}

LightEngine::InternalTask LightEngine::DequeueIncrease() noexcept {
  assert(dequeue_increase_pos_ < enqueue_increase_pos_);
  return increase_queue_[dequeue_increase_pos_++];
}

LightEngine::InternalTask LightEngine::DequeueDecrease() noexcept {
  assert(dequeue_decrease_pos_ < enqueue_decrease_pos_);
  return decrease_queue_[dequeue_decrease_pos_++];
}

bool LightEngine::IsIncreaseEmpty() const noexcept {
  return enqueue_increase_pos_ == dequeue_increase_pos_;
}

bool LightEngine::IsDecreaseEmpty() const noexcept {
  return enqueue_decrease_pos_ == dequeue_decrease_pos_;
}

bool LightEngine::TryDequeueIncrease(InternalTask& task) noexcept {
  if (IsIncreaseEmpty()) return false;
  task = DequeueIncrease();
  return true;
}

bool LightEngine::TryDequeueDecrease(InternalTask& task) noexcept {
  if (IsDecreaseEmpty()) return false;
  task = DequeueDecrease();
  return true;
}

void LightEngine::ResetIncreaseQueue() {
  enqueue_increase_pos_ = 0;
  dequeue_increase_pos_ = 0;
  increase_queue_.resize(kQueueSizeIncrement);
}

void LightEngine::ResetDecreaseQueue() {
  enqueue_decrease_pos_ = 0;
  dequeue_decrease_pos_ = 0;
  decrease_queue_.resize(kQueueSizeIncrement);
}

void LightEngine::SetLightLvl(BlockPos block_pos, int light_lvl) {
  if (type_ == EngineType::kBlockLight) {
    light_cache_->SetBlockLight(block_pos, light_lvl);
  } else {
    light_cache_->SetSkyLight(block_pos, light_lvl);
  }
}

int LightEngine::GetLightLvl(BlockPos block_pos) {
  int lvl = 0;
  if (type_ == EngineType::kBlockLight) {
    lvl = light_cache_->GetBlockLight(block_pos);
  } else {
    lvl = light_cache_->GetSkyLight(block_pos);
  }
  return lvl;
}

BlockID LightEngine::GetBlock(BlockPos pos) {
  return light_cache_->GetBlock(pos);
}

bool LightEngine::CheckChunk(ChunkPos pos) {
  return light_cache_->CheckChunk(pos);
}

Chunk* LightEngine::GetChunk(ChunkPos pos) {
  return light_cache_->GetChunk(pos);
}

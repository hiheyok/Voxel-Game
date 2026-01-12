#include "Level/Light/ThreadedLightEngine.h"

#include <cstddef>
#include <functional>
#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Core/Position/PositionTypes.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/BlockLightEngine.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/Light/LightEngineStats.h"
#include "Level/Light/SkyLightEngine.h"
#include "Level/World/WorldInterface.h"
#include "Utils/ThreadPool.h"
#include "Utils/Timer/Timer.h"

using std::make_unique;

ThreadedLightEngine::ThreadedLightEngine(GameContext& context,
                                         WorldInterface& world)
    : context_{context},
      world_{world},
      updater_{make_unique<ThreadPool<ChunkLightTask, int>>(
          context.options_->light_engine_threads_, "Light Updater",
          std::bind_front(&ThreadedLightEngine::WorkerUpdater, this), 250)},
      lighter_{make_unique<ThreadPool<ChunkPos, int>>(
          context.options_->light_engine_threads_, "Lighter",
          std::bind_front(&ThreadedLightEngine::WorkerLighter, this), 250)} {
  for (size_t i = 0; i < context_.options_->light_engine_threads_; ++i) {
    // Separate engines for each pool to avoid racing on SetCache()
    updater_skylight_engines_.emplace_back(
        make_unique<SkyLightEngine>(context_, world_));
    updater_blocklight_engines_.emplace_back(
        make_unique<BlockLightEngine>(context_, world_));
    lighter_skylight_engines_.emplace_back(
        make_unique<SkyLightEngine>(context_, world_));
    lighter_blocklight_engines_.emplace_back(
        make_unique<BlockLightEngine>(context_, world_));
  }
}

ThreadedLightEngine::~ThreadedLightEngine() = default;

void ThreadedLightEngine::LightUpChunk(ChunkPos pos) {
  {
    std::lock_guard<std::mutex> lock(pending_lighter_mutex_);
    if (!pending_lighter_chunks_.insert(pos).second) {
      ++duplicates_skipped_;  // Track duplicate
      return;  // Already queued, skip duplicate
    }
  }
  lighter_->SubmitTask(pos);
}
void ThreadedLightEngine::UpdateChunk(const ChunkLightTask& task) {
  {
    std::lock_guard<std::mutex> lock(pending_updater_mutex_);
    if (!pending_updater_chunks_.insert(task.GetChunkPos()).second) {
      ++duplicates_skipped_;  // Track duplicate
      return;  // Already queued, skip duplicate
    }
  }
  updater_->SubmitTask(task);
}

size_t ThreadedLightEngine::GetQueueSize() const noexcept {
  return updater_->GetQueueSize() + lighter_->GetQueueSize();
}

double ThreadedLightEngine::GetAverageLightUpTime() const noexcept {
  if (light_update_done_ == 0) {
    return -1;
  }

  return total_cpu_time_ / light_update_done_;
}

LightEngineStats ThreadedLightEngine::GetStats() const noexcept {
  LightEngineStats stats;
  stats.average_light_update_time_ = GetAverageLightUpTime();
  stats.queue_size_ = GetQueueSize();
  stats.duplicates_skipped_ = duplicates_skipped_.load();
  return stats;
}

int ThreadedLightEngine::WorkerUpdater(const ChunkLightTask& tasks,
                                       int worker_id) {
  SkyLightEngine& skylight_engine = *updater_skylight_engines_[worker_id];
  BlockLightEngine& blocklight_engine = *updater_blocklight_engines_[worker_id];

  Timer time;
  LightEngineCache cache{context_, world_};
  cache.BuildCache(tasks.GetChunkPos().GetBlockPosOffset());

  skylight_engine.SetCache(&cache);
  skylight_engine.Propagate(tasks);

  blocklight_engine.SetCache(&cache);
  blocklight_engine.Propagate(tasks);

  cache.UpdateLight();
  double cpu_time = time.GetTimePassed_us();
  total_cpu_time_ += cpu_time;
  ++light_update_done_;

  // Remove from pending set after processing
  {
    std::lock_guard<std::mutex> lock(pending_updater_mutex_);
    pending_updater_chunks_.erase(tasks.GetChunkPos());
  }
  return 987654321;
}

int ThreadedLightEngine::WorkerLighter(ChunkPos pos, int worker_id) {
  SkyLightEngine& skylight_engine = *lighter_skylight_engines_[worker_id];
  BlockLightEngine& blocklight_engine = *lighter_blocklight_engines_[worker_id];

  Timer time;
  LightEngineCache cache{context_, world_};
  cache.BuildCache(pos.GetBlockPosOffset());

  skylight_engine.SetCache(&cache);
  skylight_engine.LightChunk(pos);

  blocklight_engine.SetCache(&cache);
  blocklight_engine.LightChunk(pos);

  cache.UpdateLight();
  double cpu_time = time.GetTimePassed_us();
  total_cpu_time_ += cpu_time;
  ++light_update_done_;

  // Remove from pending set after processing
  {
    std::lock_guard<std::mutex> lock(pending_lighter_mutex_);
    pending_lighter_chunks_.erase(pos);
  }
  return 987654321;
}

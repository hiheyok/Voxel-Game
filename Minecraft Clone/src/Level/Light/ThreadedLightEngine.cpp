#include "Level/Light/ThreadedLightEngine.h"

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Light/BlockLightEngine.h"
#include "Level/Light/ChunkLightTask.h"
#include "Level/Light/LightEngineCache.h"
#include "Level/Light/LightEngineStats.h"
#include "Level/Light/SkyLightEngine.h"
#include "Level/World/WorldInterface.h"
#include "Utils/ThreadPool.h"
#include "Utils/Timer/Timer.h"

ThreadedLightEngine::ThreadedLightEngine(GameContext& game_context,
                                         WorldInterface& world)
    : game_context_{game_context},
      world_{world},
      updater_{std::make_unique<ThreadPool<ChunkLightTask, int>>(
          game_context.options_->light_engine_threads_, "Light Updater",
          std::bind_front(&ThreadedLightEngine::WorkerUpdater, this), 250)},
      lighter_{std::make_unique<ThreadPool<ChunkPos, int>>(
          game_context.options_->light_engine_threads_, "Lighter",
          std::bind_front(&ThreadedLightEngine::WorkerLighter, this), 250)} {}

ThreadedLightEngine::~ThreadedLightEngine() = default;

void ThreadedLightEngine::LightUpChunk(ChunkPos pos) {
  lighter_->SubmitTask(pos);
}
void ThreadedLightEngine::UpdateChunk(const ChunkLightTask& task) {
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
  return stats;
}

int ThreadedLightEngine::WorkerUpdater(const ChunkLightTask& tasks) {
  thread_local SkyLightEngine skylight_engine{game_context_, world_};
  thread_local BlockLightEngine blocklight_engine{game_context_, world_};

  Timer time;
  LightEngineCache cache{game_context_, world_};
  cache.BuildCache(tasks.GetChunkPos().GetBlockPosOffset());

  skylight_engine.SetCache(&cache);
  skylight_engine.Propagate(tasks);

  blocklight_engine.SetCache(&cache);
  blocklight_engine.Propagate(tasks);

  cache.UpdateLight();
  double cpu_time = time.GetTimePassed_us(); 
  total_cpu_time_ += cpu_time;
  ++light_update_done_;
  return 987654321;
}

int ThreadedLightEngine::WorkerLighter(ChunkPos pos) {
  thread_local SkyLightEngine skylight_engine{game_context_, world_};
  thread_local BlockLightEngine blocklight_engine{game_context_, world_};

  Timer time;
  LightEngineCache cache{game_context_, world_};
  cache.BuildCache(pos.GetBlockPosOffset());

  skylight_engine.SetCache(&cache);
  skylight_engine.LightChunk(pos);

  blocklight_engine.SetCache(&cache);
  blocklight_engine.LightChunk(pos);

  cache.UpdateLight();
  double cpu_time = time.GetTimePassed_us();
  total_cpu_time_ += cpu_time;
  ++light_update_done_;
  return 987654321;
}

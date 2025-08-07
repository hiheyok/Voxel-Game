#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "Utils/ThreadPool.h"

class ChunkLightTask;
class GameContext;
class WorldInterface;

struct LightEngineStats;
/*
 * Handles the lighting engine IO and thread management
 * Each thread handles both sky and block update
 */
class ThreadedLightEngine {
 public:
  explicit ThreadedLightEngine(GameContext&, WorldInterface&);
  ~ThreadedLightEngine();

  void LightUpChunk(ChunkPos);
  void UpdateChunk(const ChunkLightTask&);
  size_t GetQueueSize() const noexcept;
  double GetAverageLightUpTime() const noexcept;

  LightEngineStats GetStats() const noexcept;

 private:
  int WorkerUpdater(const ChunkLightTask&);
  // Handles newly created chunks
  int WorkerLighter(ChunkPos pos);

  GameContext& context_;
  WorldInterface& world_;

  int light_update_done_ = 0;
  double total_cpu_time_ = 0.0;

  std::unique_ptr<ThreadPool<ChunkLightTask, int>>
      updater_;  // Returns int, placeholder until i can make it so it can
  // handle void
  std::unique_ptr<ThreadPool<ChunkPos, int>>
      lighter_;  // Returns int, placeholder until i can make it so it can
                 // handle void
};

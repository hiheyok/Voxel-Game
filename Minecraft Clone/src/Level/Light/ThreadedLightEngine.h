#pragma once

#include <memory>
#include <vector>
#include <atomic>

#include "Core/Typenames.h"
#include "Utils/ThreadPool.h"

class ChunkLightTask;
class GameContext;
class WorldInterface;

class LightEngine;

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

 private:
  int WorkerUpdater(const ChunkLightTask&);
  // Handles newly created chunks
  int WorkerLighter(ChunkPos pos);

  GameContext& game_context_;
  WorldInterface& world_;

  std::unique_ptr<ThreadPool<ChunkLightTask, int>>
      updater_;  // Returns int, placeholder until i can make it so it can
  // handle void
  std::unique_ptr<ThreadPool<ChunkPos, int>>
      lighter_;  // Returns int, placeholder until i can make it so it can
                 // handle void
};

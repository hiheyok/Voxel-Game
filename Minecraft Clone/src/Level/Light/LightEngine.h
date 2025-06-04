// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>
#include <type_traits>
#include <vector>

#include "Core/Typenames.h"
#include "Utils/ThreadPool.h"

class LightStorage;
class HeightMap;
class Chunk;
class WorldInterface;
class GameContext;

class LightEngine {
 public:
  LightEngine(GameContext&, size_t thread_count, WorldInterface* interface);
  ~LightEngine();

  LightEngine(const LightEngine&) = delete;
  LightEngine(LightEngine&&) = delete;
  LightEngine& operator=(const LightEngine&) = delete;
  LightEngine& operator=(LightEngine&&) = delete;

  void Generate(const std::vector<ChunkPos>& IDs);

  std::vector<std::unique_ptr<LightStorage>> GetOutput();

  void QueueChunk(ChunkPos pos);

  size_t GetQueueSize();

 private:
  std::unique_ptr<LightStorage> Worker(ChunkPos pos);

  using WorkerReturnType = std::invoke_result_t<decltype(&LightEngine::Worker),
                                                LightEngine*, ChunkPos>;

  std::unique_ptr<ThreadPool<ChunkPos, WorkerReturnType>> lighting_thread_pool_;
  WorldInterface* world_;

  void IncreaseLightLevel(std::unique_ptr<LightStorage>& container, uint8_t lvl,
                          int x, int y, int z);

  void WorkOnChunkSkylight(Chunk* chunk, std::unique_ptr<LightStorage>& light);

  GameContext& game_context_;
  std::unique_ptr<LightStorage> SkyLighting(ChunkPos pos);
};

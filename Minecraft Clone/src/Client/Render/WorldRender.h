// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "Utils/ThreadPool.h"
#include "WorldRenderInfo.h"

class TerrainRenderer;
class ClientCache;
class PerformanceProfiler;
class PlayerPOV;

struct GLFWwindow;

namespace Mesh {
struct ChunkVertexData;
}

class WorldRender : public WorldRenderInfo {
 public:
  explicit WorldRender(PlayerPOV* player);
  ~WorldRender();

  WorldRender(const WorldRender&) = delete;
  WorldRender(WorldRender&&) = delete;
  WorldRender& operator=(const WorldRender&) = delete;
  WorldRender& operator=(WorldRender&&) = delete;

  void Render();

  void LoadChunkToRenderer(ChunkPos chunk);

  void Start(GLFWwindow* window, ClientCache* cache,
             PerformanceProfiler* profiler);

  void Update(std::vector<ChunkPos> updatedChunks);

  size_t GetQueuedSize();

  std::unique_ptr<TerrainRenderer> renderer_;
  PerformanceProfiler* profiler_;

 private:
  void LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks);
  std::unique_ptr<Mesh::ChunkVertexData> Worker(ChunkPos pos);

  using WorkerReturnType = std::invoke_result_t<decltype(&WorldRender::Worker),
                                                WorldRender*, ChunkPos>;

  std::unique_ptr<ThreadPool<ChunkPos, WorkerReturnType>> mesh_thread_pool_;
  std::vector<std::unique_ptr<Mesh::ChunkVertexData>> mesh_add_queue_;

  PlayerPOV* player_;
  GLFWwindow* window_;
  ClientCache* cache_;
};

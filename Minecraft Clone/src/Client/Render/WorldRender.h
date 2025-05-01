#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>

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
  WorldRender(PlayerPOV* player);
  ~WorldRender();

  void Render();

  void LoadChunkToRenderer(ChunkPos chunk);

  void Start(GLFWwindow* window, ClientCache* cache,
             PerformanceProfiler* profiler);

  void Stop();

  void Update(std::vector<ChunkPos> updatedChunks);

  size_t GetQueuedSize();

  std::unique_ptr<TerrainRenderer> renderer_;
  PerformanceProfiler* profiler_;

 private:
  void LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks);
  std::unique_ptr<Mesh::ChunkVertexData> Worker(ChunkPos pos);

  std::unique_ptr<ThreadPool<ChunkPos, std::unique_ptr<Mesh::ChunkVertexData>>>
      mesh_thread_pool_;
  std::vector<std::unique_ptr<Mesh::ChunkVertexData>> mesh_add_queue_;

  PlayerPOV* player_;
  GLFWwindow* window_;
  ClientCache* cache_;
};

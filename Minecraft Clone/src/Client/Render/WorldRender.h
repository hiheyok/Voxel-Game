#pragma once
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "WorldRenderInfo.h"
#include "../../Utils/ThreadPool.h"
#include "../../Level/Typenames.h"

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
    WorldRender();
    ~WorldRender();

    void SetRotation(glm::dvec2 rotation);

    void SetPosition(glm::dvec3 position);

    void Render();

    void LoadChunkToRenderer(ChunkPos chunk);


    void Start(GLFWwindow* window, ClientCache* cache, PerformanceProfiler* profiler);

    void Stop();

    void Update(std::vector<ChunkPos> updatedChunks);

    size_t GetQueuedSize();

    std::unique_ptr<TerrainRenderer> renderer_;
    PerformanceProfiler* profiler_;
private:
    void LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks);
    static std::unique_ptr<Mesh::ChunkVertexData> Worker(const ChunkPos& pos);
    
    std::unique_ptr<ThreadPool<ChunkPos, std::unique_ptr<Mesh::ChunkVertexData>, WorldRender::Worker>> mesh_thread_pool_;
    std::vector<std::unique_ptr<Mesh::ChunkVertexData>> mesh_add_queue_;

    std::unique_ptr<PlayerPOV> player_;
    GLFWwindow* window_;
    static ClientCache* cache_;
};

inline ClientCache* WorldRender::cache_ = nullptr;
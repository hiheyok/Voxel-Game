#pragma once
#include <memory>

#include "Core/Typenames.h"

class Timer;
class Camera;
class Shader;
class ChunkDrawBatch;

struct GLFWwindow;

namespace Mesh {
    struct ChunkVertexData;
}

class TerrainRenderer {
private:
    void AddChunk(const ChunkPos& pos, const std::vector<uint32_t>& data, std::vector<ChunkDrawBatch>& BatchType, FastHashMap<ChunkPos, int>& LookUpMap);

    void CreateNewSolidBatch();

    void CreateNewTransparentBatch();

    int horizontal_render_distance_ = 16;
    int vertical_render_distance_ = 16;
    float fov_ = 80.f;
    int TextureAminationIndex = 0;

    std::vector<ChunkDrawBatch> chunk_solid_batches_;
    FastHashMap<ChunkPos, int> chunk_batch_solid_lookup_; //f: ChunkPos -> SolidBatchIndex

    std::vector<ChunkDrawBatch> chunk_transparent_batches_;
    FastHashMap<ChunkPos, int> chunk_batch_transparent_lookup_; //f: ChunkPos -> TransparentBatchIndex

    GLFWwindow* window_ = nullptr;
    std::unique_ptr<Shader> cubic_shader_;
    Camera* camera_;
    std::unique_ptr<Timer> time_;
public:
    TerrainRenderer();
    ~TerrainRenderer();

    void Initialize(GLFWwindow* window_, Camera* camera_);

    void PrepareRenderer();

    void SetupCallSolid();

    void SetupCallTransparent();

    void RenderSky();

    void Render();

    void Defrag(int iterations);

    void Update();

    void setSettings(uint32_t RenderDistance, uint32_t verticalRenderDistance, float fov_);

    void LoadAssets();

    void AddChunk(std::unique_ptr<Mesh::ChunkVertexData> MeshData);

    double getDebugTime();

    double getFragmentationRate();

    size_t getVRAMUsageFull();

    void Cleanup();

    size_t amountOfMeshGenerated = 1;
};


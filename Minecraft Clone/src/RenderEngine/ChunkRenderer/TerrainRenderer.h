#pragma once
#include  "Mesh/ChunkMeshingV2.h"
#include "../Camera/camera.h"
#include "Batch/ChunkBatch.h"
#include "../OpenGL/Shader/Shader.h"
#include "../../Core/Options/Option.h"
#include "../../Level/Timer/Timer.h"

#include <concurrent_vector.h>
#include <mutex>

class TerrainRenderer {
public:
    TerrainRenderer() {

    }

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

private:
    void AddChunk(const ChunkPos& pos, const std::vector<uint32_t>& data, std::vector<ChunkDrawBatch>& BatchType, FastHashMap<ChunkPos, int>& LookUpMap);

    void SetupShaders();

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
    Shader cubic_shader_;
    Camera* camera_;
    Timer time_;
};


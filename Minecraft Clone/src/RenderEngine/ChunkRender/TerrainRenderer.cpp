#include "RenderEngine/ChunkRender/TerrainRenderer.h"

#include "Core/Options/Option.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/ChunkRender/Batch/ChunkBatch.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"
#include "RenderEngine/Frustum/frustum.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Texture/Texture.h"
#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"
#include "Utils/MathHelper.h"
#include "Utils/Timer/Timer.h"

TerrainRenderer::TerrainRenderer()
    : chunk_solid_batches_{},
      chunk_batch_solid_lookup_{},
      chunk_transparent_batches_{},
      chunk_batch_transparent_lookup_{},
      camera_{nullptr},
      time_{std::make_unique<Timer>()},
      cubic_shader_{std::make_unique<Shader>("assets/shaders/vert.glsl",
                                             "assets/shaders/frag.glsl")} {}

TerrainRenderer::~TerrainRenderer() = default;

void TerrainRenderer::Initialize(GLFWwindow* window, Camera* camera) {
    window_ = window;
    camera_ = camera;
    LoadAssets();

    time_->Set();

    CreateNewSolidBatch();
    CreateNewTransparentBatch();
}

void TerrainRenderer::PrepareRenderer() {
    for (ChunkDrawBatch& DrawBatch : chunk_solid_batches_) {
        DrawBatch.GenDrawCommands(horizontal_render_distance_,
                                  vertical_render_distance_);
    }

    for (ChunkDrawBatch& DrawBatch : chunk_transparent_batches_) {
        DrawBatch.GenDrawCommands(horizontal_render_distance_,
                                  vertical_render_distance_);
    }
}

void TerrainRenderer::SetupCallSolid() {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
}

void TerrainRenderer::SetupCallTransparent() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
}

void TerrainRenderer::RenderSky() {
    glClearColor(0.46274509803f, 0.65882352941f, 1.0f, 1.0);
}

void TerrainRenderer::Render() {
    LoadAssets();
    SetupCallSolid();

    for (ChunkDrawBatch& DrawBatch : chunk_solid_batches_) {
        DrawBatch.Draw(cubic_shader_.get());
    }

    SetupCallTransparent();

    for (ChunkDrawBatch& DrawBatch : chunk_transparent_batches_) {
        DrawBatch.Draw(cubic_shader_.get());
    }

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

void TerrainRenderer::Defrag(int iterations) {
    for (auto& DrawBatch : chunk_solid_batches_) {
        DrawBatch.Defrag(iterations);
    }

    for (auto& DrawBatch : chunk_solid_batches_) {
        DrawBatch.Defrag(iterations);
    }
}

void TerrainRenderer::Update() {
    int width, height;

    glfwGetWindowSize(window_, &width, &height);
    glm::mat4 model = glm::mat4(1.f);

    camera_->screen_res_ = glm::vec2(width, height);

    glm::mat4 view = camera_->GetViewMatrix();

    int x = width;
    int y = height;
    glm::mat4 projection = glm::perspective(
        glm::radians(camera_->fov_), (float)x / (float)y, 0.1f, 1000000.0f);
    cubic_shader_->Use();

    float clrMultiplier = 1.4f;

    cubic_shader_->SetMat4("view", view)
        .SetMat4("model", model)
        .SetMat4("projection", projection)
        .SetFloat("RenderDistance",
                  (float)(horizontal_render_distance_ * kChunkDim))
        .SetFloat("VerticalRenderDistance",
                  (float)(vertical_render_distance_ * kChunkDim))
        .SetVec3("camPos", camera_->position_)
        .SetVec3("tintColor", glm::vec3(0.40828402 * clrMultiplier,
                                        0.5917159 * clrMultiplier,
                                        0.2781065 * clrMultiplier))
        .SetInt("TextureAimIndex", TextureAminationIndex);

    if (time_->GetTimePassed_ms() > 100) {
        time_->Set();
        TextureAminationIndex++;

        if (TextureAminationIndex == (1 << 15)) {
            TextureAminationIndex = 0;
        }
    }
}

void TerrainRenderer::SetSettings(uint32_t renderDistance,
                                  uint32_t verticalRenderDistance, float fov) {
    horizontal_render_distance_ = renderDistance;
    vertical_render_distance_ = verticalRenderDistance;
    fov_ = fov;
}

void TerrainRenderer::LoadAssets() {
    cubic_shader_->BindTexture2D(0, g_blocks.block_texture_atlas_->Get(),
                                 "BlockTexture");
}

void TerrainRenderer::AddChunk(const ChunkPos& pos,
                               const std::vector<uint32_t>& data,
                               std::vector<ChunkDrawBatch>& batchType,
                               FastHashMap<ChunkPos, int>& lookUpMap) {
    if (lookUpMap.count(pos)) {
        size_t BatchIndex = lookUpMap[pos];
        batchType[BatchIndex].DeleteChunkVertices(pos);
        lookUpMap.erase(pos);
    }

    if (data.size() == 0) return;

    bool success = false;

    for (int batchIndex = 0; batchIndex < batchType.size(); batchIndex++) {
        size_t meshDataSize = data.size() * sizeof(uint32_t);

        if (batchType[batchIndex].memory_pool_.memory_pool_.FindFreeSpace(
                meshDataSize) == ULLONG_MAX)
            continue;

        bool InsertSuccess = batchType[batchIndex].AddChunkVertices(data, pos);

        if (!InsertSuccess) continue;

        lookUpMap.emplace(pos, batchIndex);
        success = true;
        break;
    }

    if (!success) {
        g_logger.LogInfo("TerrainRenderer::AddChunk",
                         "Unable to add chunk. Solid buffers are full!");
    }
}

void TerrainRenderer::AddChunk(
    std::unique_ptr<Mesh::ChunkVertexData> MeshData) {
    AddChunk(MeshData->position_, MeshData->solid_vertices_,
             chunk_solid_batches_, chunk_batch_solid_lookup_);
    AddChunk(MeshData->position_, MeshData->transparent_vertices_,
             chunk_transparent_batches_, chunk_batch_transparent_lookup_);
}

double TerrainRenderer::GetDebugTime() {
    double t = 0.0;

    for (int batchIndex = 0; batchIndex < chunk_solid_batches_.size();
         batchIndex++) {
        t += chunk_solid_batches_[batchIndex].debug_time_;
    }

    return t;
}

double TerrainRenderer::GetFragmentationRate() {
    size_t n = chunk_solid_batches_.size();

    double fragRate = 0;

    for (size_t batchIndex = 0; batchIndex < n; batchIndex++) {
        const auto& batch = chunk_solid_batches_[batchIndex];

        if (batch.render_list_.size() != 0) {
            fragRate += (batch.memory_pool_.statistics_.fragmentation_rate_ /
                         (double)n);
        }
    }

    return 1.0 - fragRate;
}

size_t TerrainRenderer::GetVRAMUsageFull() {
    size_t memUsage = 0;

    for (int batchIndex = 0; batchIndex < chunk_solid_batches_.size();
         batchIndex++) {
        if (chunk_solid_batches_[batchIndex].render_list_.size() == 0) {
            continue;
        }

        memUsage += chunk_solid_batches_[batchIndex]
                        .memory_pool_.statistics_.full_memory_usage_;
    }

    return memUsage;
}

void TerrainRenderer::CreateNewSolidBatch() {
    chunk_solid_batches_.emplace_back(g_app_options.solid_buffer_size_);
    size_t i = chunk_solid_batches_.size() - 1;
    chunk_solid_batches_[i].SetupBuffers();
    chunk_solid_batches_[i].camera_ = camera_;
}

void TerrainRenderer::CreateNewTransparentBatch() {
    chunk_transparent_batches_.emplace_back(
        g_app_options.transparent_buffer_size_);
    size_t i = chunk_transparent_batches_.size() - 1;
    chunk_transparent_batches_[i].SetupBuffers();
    chunk_transparent_batches_[i].camera_ = camera_;
}
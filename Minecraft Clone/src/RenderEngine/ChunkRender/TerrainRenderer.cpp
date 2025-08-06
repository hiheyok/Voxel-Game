// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/TerrainRenderer.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "RenderEngine/BlockModel/BlockModelManager.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/ChunkRender/Batch/ChunkBatch.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"
#include "RenderEngine/ChunkRender/Mesh/ChunkMesh.h"
#include "RenderEngine/Frustum/frustum.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Texture/Texture.h"
#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"
#include "Utils/MathHelper.h"
#include "Utils/Timer/Timer.h"

TerrainRenderer::TerrainRenderer(GameContext& game_context)
    : game_context_{game_context},
      chunk_solid_batches_{},
      chunk_batch_solid_lookup_{},
      chunk_transparent_batches_{},
      chunk_batch_transparent_lookup_{},
      camera_{nullptr},
      time_{std::make_unique<Timer>()},
      cubic_shader_{std::make_unique<Shader>(game_context,
                                             "assets/shaders/vert.glsl",
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

  camera_->screen_res_ = {width, height};

  glm::mat4 view = camera_->GetViewMatrix();

  int x = width;
  int y = height;
  glm::mat4 projection = glm::perspective(
      glm::radians(camera_->fov_), static_cast<float>(x) / y, 0.1f, 1000000.0f);
  cubic_shader_->Use();

  cubic_shader_->SetMat4("view", view)
      .SetMat4("model", model)
      .SetMat4("projection", projection)
      .SetFloat("RenderDistance",
                static_cast<float>(horizontal_render_distance_ * kChunkDim))
      .SetFloat("VerticalRenderDistance",
                static_cast<float>(vertical_render_distance_ * kChunkDim))
      .SetVec3("camPos", camera_->position_)
      .SetVec3("tintColor", {152.0 / 255, 189.0 / 255, 83.0 / 255})
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
  cubic_shader_->BindTexture2D(
      0, game_context_.block_model_manager_->GetTextureAtlasID(),
      "BlockTexture");
  cubic_shader_->BindTexture2D(
      1, game_context_.block_model_manager_->GetLightMapID(), "LightMap");
}

void TerrainRenderer::AddChunk(ChunkPos pos,
                               const std::vector<BlockVertexFormat>& data,
                               std::vector<ChunkDrawBatch>& batchType,
                               FastHashMap<ChunkPos, int>& lookUpMap) {
  if (lookUpMap.count(pos)) {
    size_t BatchIndex = lookUpMap[pos];
    batchType[BatchIndex].DeleteChunkVertices(pos);
    lookUpMap.erase(pos);
  }

  if (data.size() == 0) return;

  bool success = false;

  for (int batchIndex = 0; batchIndex < static_cast<int>(batchType.size());
       batchIndex++) {
    size_t mesh_data_size = data.size() * sizeof(data[0]);

    if (batchType[batchIndex].memory_pool_.memory_pool_.FindFreeSpace(
            mesh_data_size) == ULLONG_MAX)
      continue;

    bool insert_success = batchType[batchIndex].AddChunkVertices(data, pos);

    if (!insert_success) continue;

    lookUpMap.emplace(pos, batchIndex);
    success = true;
    break;
  }

  if (!success) {
    game_context_.logger_->LogInfo(
        "TerrainRenderer::AddChunk",
        "Unable to add chunk. Solid buffers are full!");
  }
}

void TerrainRenderer::AddChunk(
    std::unique_ptr<Mesh::ChunkVertexData> MeshData) {
  AddChunk(MeshData->position_, MeshData->solid_vertices_, chunk_solid_batches_,
           chunk_batch_solid_lookup_);
  AddChunk(MeshData->position_, MeshData->transparent_vertices_,
           chunk_transparent_batches_, chunk_batch_transparent_lookup_);
}

double TerrainRenderer::GetDebugTime() {
  double t = 0.0;

  for (size_t batchIndex = 0; batchIndex < chunk_solid_batches_.size();
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
      fragRate += (batch.memory_pool_.statistics_.fragmentation_rate_ / n);
    }
  }

  return 1.0 - fragRate;
}

size_t TerrainRenderer::GetVRAMUsageFull() {
  size_t memUsage = 0;

  for (size_t batchIndex = 0; batchIndex < chunk_solid_batches_.size();
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
  chunk_solid_batches_.emplace_back(game_context_,
                                    game_context_.options_->solid_buffer_size_);
  size_t i = chunk_solid_batches_.size() - 1;
  chunk_solid_batches_[i].SetupBuffers();
  chunk_solid_batches_[i].camera_ = camera_;
}

void TerrainRenderer::CreateNewTransparentBatch() {
  chunk_transparent_batches_.emplace_back(
      game_context_, game_context_.options_->transparent_buffer_size_);
  size_t i = chunk_transparent_batches_.size() - 1;
  chunk_transparent_batches_[i].SetupBuffers();
  chunk_transparent_batches_[i].camera_ = camera_;
}

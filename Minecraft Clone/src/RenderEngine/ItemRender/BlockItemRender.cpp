// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ItemRender/BlockItemRender.h"

#include <memory>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Assets/AssetManager.h"
#include "Assets/Types/Models/Data/RenderableModel.h"
#include "Assets/Types/Models/Managers/BlockModelManager.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Render/RenderDrawElements.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"

BlockItemRender::BlockItemRender(GameContext& context)
    : context_{context},
      render_{std::make_unique<RenderDrawElements>(context)},
      camera_{std::make_unique<Camera>()} {}
BlockItemRender::~BlockItemRender() = default;

void BlockItemRender::Initialize() {
  render_->SetShader("block_render");

  render_->SetDataAttribute(0, 3, GL_FLOAT, 6, 0);
  render_->SetDataAttribute(1, 2, GL_FLOAT, 6, 3);
  render_->SetDataAttribute(2, 1, GL_FLOAT, 6, 5);

  SetCamera();

  float dimensions = 0.85f;

  glm::mat4 view = camera_->GetViewMatrix();
  glm::mat4 modelMat = glm::mat4(1.f);
  glm::mat4 orthoProj = glm::ortho(-dimensions, dimensions, -dimensions,
                                   dimensions, 0.001f, 3.0f);
  render_->GetShader()
      ->SetMat4("view", view)
      .SetMat4("model", modelMat)
      .SetMat4("projection", orthoProj);

  RenderHandle<TextureAtlas> atlas =
      context_.render_resource_manager_->GetAtlas("blocks");

  render_->SetTexture(0, atlas);
}

void BlockItemRender::RenderBlock(Item item) {
  std::vector<VertexFormat> vertices{};
  std::vector<uint32_t> indices{};
  std::vector<Block*> blocks = context_.blocks_->block_type_data_;

  const BlockModelManager& model_manager =
      context_.assets_->GetBlockModelManager();
  const BlockModelManager::ModelList& models = model_manager.GetModels();

  AssetHandle<RenderableModel> model = models[item.GetBlock()];

  ExtractVertices(model, vertices, indices);
  render_->SetData(vertices, indices);
  render_->SetIndicesCount(indices.size());

  setDrawCalls();
  render_->Render();
}

void BlockItemRender::ExtractVertices(AssetHandle<RenderableModel> model,
                                      std::vector<VertexFormat>& vertices,
                                      std::vector<uint32_t>& indices) {
  if (!model.HasValue()) {
    return;  // Render nothing
  }

  for (const auto& element : model->GetElements()) {
    for (auto direction : Directions<BlockPos>()) {
      // TODO(hiheyok): Temp fix
      if (direction == 1 || direction == 3 || direction == 5) continue;
      if (!element.faces_[direction].has_value()) {
        continue;
      }

      int axis = direction.GetAxis();
      int axis_u = (axis + 1) % 3;
      int axis_v = (axis + 2) % 3;
      int side = direction.GetDirection();

      int slice_mask = (1 << axis) * (~side & 1);
      int u_mask = 1 << axis_u;
      int v_mask = 1 << axis_v;

      VertexFormat v_00, v_01, v_10, v_11;

      v_00.pos_ = element.corners_[slice_mask];
      v_01.pos_ = element.corners_[slice_mask | v_mask];
      v_10.pos_ = element.corners_[slice_mask | u_mask];
      v_11.pos_ = element.corners_[slice_mask | v_mask | u_mask];

      const baked_model::Face& face = element.faces_[direction].value();

      v_00.uv_ = face.uv_00_;
      v_01.uv_ = face.uv_01_;
      v_10.uv_ = face.uv_10_;
      v_11.uv_ = face.uv_11_;

      float light;

      if (axis == 0) {
        light = 8.0f;
      } else if (axis == 1) {
        light = 15.0f;
      } else {
        light = 12.0f;
      }

      v_00.light_ = light;
      v_01.light_ = light;
      v_10.light_ = light;
      v_11.light_ = light;

      uint32_t vertex_idx = vertices.size();

      vertices.insert(vertices.end(), {v_00, v_01, v_10, v_11});
      indices.insert(indices.end(),
                     {vertex_idx + 0, vertex_idx + 1, vertex_idx + 2,
                      vertex_idx + 2, vertex_idx + 1, vertex_idx + 3});
    }
  }
}

void BlockItemRender::SetCamera() {
  camera_->fov_ = 57;
  camera_->position_ = {1.1f, 1.1f, 1.1f};
  camera_->pitch_ = -35;
  camera_->yaw_ = -135;

  camera_->UpdateCameraVectors();
}

void BlockItemRender::setDrawCalls() {
  glEnable(GL_BLEND);
  glDepthMask(GL_TRUE);
  // glDisable(GL_CULL_FACE);
}

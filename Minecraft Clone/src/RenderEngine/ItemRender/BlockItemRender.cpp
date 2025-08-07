// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ItemRender/BlockItemRender.h"

#include <memory>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Block.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"
#include "RenderEngine/BlockModel/BlockModelManager.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Render/RenderDrawElements.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"

BlockItemRender::BlockItemRender(GameContext& context)
    : context_{context},
      render_{std::make_unique<RenderDrawElements>(context)},
      camera_{std::make_unique<Camera>()} {}
BlockItemRender::~BlockItemRender() = default;

void BlockItemRender::Initialize() {
  render_->SetShader("block_render");

  render_->SetDataAttribute(0, 3, GL_FLOAT, 7, 0);
  render_->SetDataAttribute(1, 2, GL_FLOAT, 7, 3);
  render_->SetDataAttribute(2, 1, GL_FLOAT, 7, 5);
  render_->SetDataAttribute(3, 1, GL_FLOAT, 7, 6);

  SetCamera();

  float dimensions = 0.85f;

  glm::mat4 view = camera_->GetViewMatrix();
  glm::mat4 modelMat = glm::mat4(1.f);
  glm::mat4 orthoProj = glm::ortho(-dimensions, dimensions, -dimensions,
                                   dimensions, 0.001f, 3.0f);
  render_->GetShader()
      .SetMat4("view", view)
      .SetMat4("model", modelMat)
      .SetMat4("projection", orthoProj);
  uint32_t texture_atlas = context_.block_model_manager_->GetTextureAtlasID();
  render_->SetTexture2D(0, texture_atlas, "BlockTexture");
}

void BlockItemRender::RenderBlock(Item item) {
  std::vector<float> vertices{};
  std::vector<uint32_t> indices{};

  context_.block_model_manager_->GetBlockModel(item.GetBlock())
      .GetVertices(vertices, indices);
  render_->SetData(vertices, indices);

  render_->SetIndicesCount(indices.size());
  setDrawCalls();
  render_->Render();
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
  glDisable(GL_CULL_FACE);
}

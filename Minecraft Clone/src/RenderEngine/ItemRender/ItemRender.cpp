#include "RenderEngine/ItemRender/ItemRender.h"

#include "Assets/AssetManager.h"
#include "Assets/Types/Models/Data/RenderableModel.h"
#include "Assets/Types/Models/Managers/BlockModelManager.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Item/Item.h"
#include "RenderEngine/Camera/Camera.h"
#include "RenderEngine/OpenGL/Render/RenderDrawElements.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"

using std::make_unique;

ItemRender::ItemRender(GameContext& context)
    : context_{context},
      render_{make_unique<RenderDrawElements>(context)},
      camera_{make_unique<Camera>()} {
  Initialize();
}
ItemRender::~ItemRender() = default;

void ItemRender::RenderModel(const Item& item) {
  auto [index_offset, model_size] = item_model_info_[item.GetBlock()];

  // TODO(hiheyok): Add support for null model
  if (model_size == 0) {
    return;
  }

  SetDrawCalls();
  render_->SetIndicesCount(model_size);
  render_->SetIndicesOffset(index_offset * 4);
  RenderHandle<TextureAtlas> atlas =
      context_.render_resource_manager_->GetAtlas("blocks");

  render_->SetTexture(0, atlas);
  render_->Render();
}

void ItemRender::Initialize() {
  SetUpRenderer();
  SetUpData();
}

void ItemRender::SetUpRenderer() {
  render_->SetShader("item_render");
  render_->SetDataAttribute(0, 3, GL_FLOAT, 6, 0);
  render_->SetDataAttribute(1, 2, GL_FLOAT, 6, 3);
  render_->SetDataAttribute(2, 1, GL_FLOAT, 6, 5);

  camera_->fov_ = 57;
  camera_->position_ = {1.1f, 1.1f, 1.1f};
  camera_->pitch_ = -35;
  camera_->yaw_ = -135;
  camera_->UpdateCameraVectors();

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

void ItemRender::SetUpData() {
  const BlockModelManager::ModelList& models =
      context_.assets_->GetBlockModelManager().GetModels();

  std::vector<VertexFormat> vertices;
  std::vector<uint32_t> indices;
  item_model_info_.resize(models.size());

  for (int id = 0; id < models.size(); ++id) {
    size_t base_index = indices.size();

    AssetHandle<RenderableModel> model = models[id];

    GetVertices(model, vertices, indices);
    size_t model_size = indices.size() - base_index;

    item_model_info_[id] = {base_index, model_size};
  }

  render_->SetData(vertices, indices);
}

void ItemRender::SetDrawCalls() {
  glEnable(GL_BLEND);
  glDepthMask(GL_TRUE);
}

void ItemRender::GetVertices(AssetHandle<RenderableModel> model,
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

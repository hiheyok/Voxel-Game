#include "RenderEngine/ItemRender/ItemRender.h"

#include <algorithm>

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

// Camera position for item rendering (used for sorting)
static constexpr glm::vec3 kCameraPos{1.1f, 1.1f, 1.1f};

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
  camera_->position_ = kCameraPos;
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);
}

void ItemRender::GetVertices(AssetHandle<RenderableModel> model,
                             std::vector<VertexFormat>& vertices,
                             std::vector<uint32_t>& indices) {
  if (!model.HasValue()) {
    return;  // Render nothing
  }

  // Collect all faces with their center positions for sorting
  struct FaceData {
    VertexFormat v_00, v_01, v_10, v_11;
    glm::vec3 center;
    float distance_sq;
  };
  std::vector<FaceData> faces;

  for (const auto& element : model->GetElements()) {
    for (auto direction : Directions<BlockPos>()) {
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

      FaceData face_data;

      face_data.v_00.pos_ = element.corners_[slice_mask];
      face_data.v_01.pos_ = element.corners_[slice_mask | v_mask];
      face_data.v_10.pos_ = element.corners_[slice_mask | u_mask];
      face_data.v_11.pos_ = element.corners_[slice_mask | v_mask | u_mask];

      const baked_model::Face& face = element.faces_[direction].value();

      face_data.v_00.uv_ = face.uv_00_;
      face_data.v_01.uv_ = face.uv_01_;
      face_data.v_10.uv_ = face.uv_10_;
      face_data.v_11.uv_ = face.uv_11_;

      float light;
      if (axis == 0) {
        light = 8.0f;
      } else if (axis == 1) {
        light = 15.0f;
      } else {
        light = 12.0f;
      }

      face_data.v_00.light_ = light;
      face_data.v_01.light_ = light;
      face_data.v_10.light_ = light;
      face_data.v_11.light_ = light;

      // Calculate face center for sorting
      face_data.center = (face_data.v_00.pos_ + face_data.v_01.pos_ +
                          face_data.v_10.pos_ + face_data.v_11.pos_) *
                         0.25f;

      // Distance squared from camera (for sorting back-to-front)
      glm::vec3 diff = face_data.center - kCameraPos;
      face_data.distance_sq = glm::dot(diff, diff);

      faces.push_back(face_data);
    }
  }

  // Sort faces back-to-front (furthest first, closest last)
  std::sort(faces.begin(), faces.end(),
            [](const FaceData& a, const FaceData& b) {
              return a.distance_sq > b.distance_sq;
            });

  // Build vertex and index buffers in sorted order
  for (const auto& face_data : faces) {
    uint32_t vertex_idx = vertices.size();

    vertices.insert(vertices.end(), {face_data.v_00, face_data.v_01,
                                     face_data.v_10, face_data.v_11});
    indices.insert(indices.end(),
                   {vertex_idx + 0, vertex_idx + 1, vertex_idx + 2,
                    vertex_idx + 2, vertex_idx + 1, vertex_idx + 3});
  }
}

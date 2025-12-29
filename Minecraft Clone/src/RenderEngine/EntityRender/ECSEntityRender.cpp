#include "RenderEngine/EntityRender/ECSEntityRender.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "Assets/AssetManager.h"
#include "Client/Player/PlayerPOV.h"
#include "Core/GameContext/GameContext.h"
#include "Client/ECS/ClientECSManager.h"
#include "Client/ECS/ClientEntitySystems.h"
#include "Level/ECS/ComponentMap.h"
#include "Level/ECS/IECSManager.h"
#include "Level/ECS/Systems/ITransformSystem.h"
#include "Level/Entity/Entities.h"

#include "Level/Entity/EntityTypeData.h"
#include "RenderEngine/Camera/Camera.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Render/RenderDrawElementsInstanced.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"

ECSEntityRender::ECSEntityRender(GameContext& context, IECSManager& manager,
                                 PlayerPOV* player)
    : context_{context},
      ecs_manager_{manager},
      player_{player},
      shader_{context_.render_resource_manager_->GetShader("entity_render")},
      renderer_{
          std::make_unique<RenderDrawElementsInstanced>(context, GL_TRIANGLES)},
      ssbo_pos_{std::make_unique<Buffer>(context)},
      ssbo_vel_{std::make_unique<Buffer>(context)},
      ssbo_acc_{std::make_unique<Buffer>(context)} {}


ECSEntityRender::~ECSEntityRender() = default;

void ECSEntityRender::Initialize() {
  static constexpr int kMaxEntityCount = 10000;

  const std::vector<EntityTypeData*>& entity_type_list =
      context_.entities_list_->entity_type_list_;

  for (int i = 0; i < entity_type_list.size(); i++) {
    if (entity_type_list[i] == nullptr) continue;
    const std::string& name = entity_type_list[i]->entity_name_;
    AssetHandle<EntityModel> model = context_.assets_->GetEntityModel(name);
    entity_cached_models_[entity_type_list[i]->id_] = model;
  }

  InitializeBuffers();

  position_arr_.resize(kMaxEntityCount * 3);
  velocity_arr_.resize(kMaxEntityCount * 3);
  acceleration_arr_.resize(kMaxEntityCount * 3);
}

void ECSEntityRender::InitializeBuffers() {
  for (auto& entity_model : entity_cached_models_) {
    Model::ModelData model = entity_model.second->GetVertices();

    uint32_t model_index = static_cast<uint32_t>(entity_vertices_.size() / 5);

    entity_element_index_[entity_model.first] = entity_indices_.size();
    entity_element_size_[entity_model.first] = model.indices_.size();

    LOG_DEBUG("Setting up entity model {}: vertices size={}, indices size={}, model_index={}", 
              static_cast<int>(entity_model.first), model.vertices_.size(), model.indices_.size(), model_index);

    entity_indices_.reserve(entity_indices_.size() + model.indices_.size());
    for (int i : model.indices_) {
      entity_indices_.emplace_back(model_index + i);
    }

    entity_vertices_.insert(entity_vertices_.end(), model.vertices_.begin(),
                            model.vertices_.end());
  }

  renderer_->SetData(entity_vertices_, entity_indices_);

  // vertex (3 floats) + tex_coord (2 floats)
  // Attr 0: 3 floats, stride 5, offset 0
  // Attr 1: 2 floats, stride 5, offset 3
  renderer_->SetDataAttribute(0, 3, GL_FLOAT, 5, 0);
  renderer_->SetDataAttribute(1, 2, GL_FLOAT, 5, 3);

  ssbo_pos_->SetType(GL_SHADER_STORAGE_BUFFER);
  ssbo_pos_->SetUsage(GL_DYNAMIC_COPY);
  ssbo_pos_->SetMaxSize(10000);  // Support 10k entities for now
  ssbo_pos_->InitializeData();

  ssbo_vel_->SetType(GL_SHADER_STORAGE_BUFFER);
  ssbo_vel_->SetUsage(GL_DYNAMIC_COPY);
  ssbo_vel_->SetMaxSize(10000);
  ssbo_vel_->InitializeData();

  ssbo_acc_->SetType(GL_SHADER_STORAGE_BUFFER);
  ssbo_acc_->SetUsage(GL_DYNAMIC_COPY);
  ssbo_acc_->SetMaxSize(10000);
  ssbo_acc_->InitializeData();
}

void ECSEntityRender::SetTimePastTick(double t) { time_past_tick_ = t; }

void ECSEntityRender::SetupCall() {
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);

  shader_->Use();

  int width, height;
  glfwGetWindowSize(window_, &width, &height);

  glm::mat4 model = glm::mat4(1.f);
  glm::mat4 view = player_->GetCamera()->GetViewMatrix();
  glm::mat4 projection =
      glm::perspective(glm::radians(player_->GetCamera()->fov_),
                       static_cast<float>(width) / height, 0.1f, 1000000.0f);

  shader_->SetMat4("view", view)
      .SetMat4("model", model)
      .SetMat4("projection", projection)
      .SetFloat("RenderDistance",
                static_cast<float>(horizontal_render_distance_ * kChunkDim))
      .SetFloat("VerticalRenderDistance",
                static_cast<float>(vertical_render_distance_ * kChunkDim))
      .SetVec3("camPos", player_->GetPosition())
      .SetFloat("TimeDelta", static_cast<float>(time_past_tick_));
}

void ECSEntityRender::Render() {
  SetupCall();

  renderer_->DisableBlend();
  renderer_->EnableDepthTest();
  renderer_->SetDepthMask(true);
  renderer_->DisableCullFace();

  // We need to set the shader on the renderer because RenderObject uses its own
  // shader member
  renderer_->SetShader("entity_render");

  // 1. Group entities by model ID
  FastHashMap<EntityType, std::vector<std::pair<EntityUUID, TransformComponent>>> grouped_entities;

  auto& transform_system = ecs_manager_.GetSystems().GetTransformSystem();
  
  // Cast to ClientECSManager to get entity types for rendering
  auto& client_ecs = static_cast<ClientECSManager&>(ecs_manager_);
  auto& client_systems = client_ecs.GetConcreteSystems();

  // Iterate over all entities with a TransformComponent
  for (const auto& [uuid, transform] : transform_system.GetComponentMap()) {
    EntityType type = client_systems.GetEntityType(uuid);
    grouped_entities[type].emplace_back(uuid, transform);
  }



  // 2. Render each group
  for (const auto& [model_id, entities] : grouped_entities) {
    if (entities.empty()) continue;

    // Prepare instance data
    size_t count = entities.size();

    for (size_t i = 0; i < count; ++i) {
      const auto& transform = entities[i].second;

      position_arr_[i * 3 + 0] = transform.position_.x;
      position_arr_[i * 3 + 1] = transform.position_.y;
      position_arr_[i * 3 + 2] = transform.position_.z;

      velocity_arr_[i * 3 + 0] = transform.velocity_.x;
      velocity_arr_[i * 3 + 1] = transform.velocity_.y;
      velocity_arr_[i * 3 + 2] = transform.velocity_.z;

      acceleration_arr_[i * 3 + 0] = transform.acceleration_.x;
      acceleration_arr_[i * 3 + 1] = transform.acceleration_.y;
      acceleration_arr_[i * 3 + 2] = transform.acceleration_.z;
    }

    // Upload to SSBOs
    ssbo_pos_->InsertSubData(0, count * 3 * sizeof(float),
                             position_arr_.data());
    ssbo_vel_->InsertSubData(0, count * 3 * sizeof(float),
                             velocity_arr_.data());
    ssbo_acc_->InsertSubData(0, count * 3 * sizeof(float),
                             acceleration_arr_.data());

    // Bind and Draw
    const std::string& model_name =
        context_.entities_list_->entity_type_list_[static_cast<int>(model_id)]
            ->entity_name_;

    RenderHandle<Texture2DV2> texture =
        context_.render_resource_manager_->GetTexture2D(model_name);

    // Reuse the renderer's texture setting capability
    renderer_->SetTexture2D(0, texture->GetId(), "EntityTexture");

    shader_->BindBufferAsSSBO(ssbo_pos_->GetId(), 1)
        .BindBufferAsSSBO(ssbo_vel_->GetId(), 2)
        .BindBufferAsSSBO(ssbo_acc_->GetId(), 3);

    // Configure draw call
    renderer_->SetIndicesCount(entity_element_size_[model_id]);
    renderer_->SetIndicesOffset(entity_element_index_[model_id] *
                                sizeof(uint32_t));
    renderer_->SetInstancesCount(count);

    renderer_->Render();

    shader_->UnbindBufferSSBO(3).UnbindBufferSSBO(2).UnbindBufferSSBO(1);
  }
}

void ECSEntityRender::SetWindow(GLFWwindow* win) { window_ = win; }

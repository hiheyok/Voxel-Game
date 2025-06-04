// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/EntityRender/MultiEntityRender.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

#include "Client/Player/PlayerPOV.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Core/GameContext/GameContext.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/EntityType.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "RenderEngine/Camera/camera.h"
#include "RenderEngine/EntityRender/EntityRenderCache.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"

MultiEntityRender::MultiEntityRender(GameContext& game_context,
                                     PlayerPOV* player)
    : game_context_{game_context},
      vbo_{std::make_unique<Buffer>(game_context)},
      ebo_{std::make_unique<Buffer>(game_context)},
      ssbo_pos_{std::make_unique<Buffer>(game_context)},
      ssbo_vel_{std::make_unique<Buffer>(game_context)},
      ssbo_acc_{std::make_unique<Buffer>(game_context)},
      vao_{std::make_unique<VertexArray>(game_context)},
      player_{player},
      shader_{std::make_unique<Shader>(
          game_context, "assets/shaders/Entity/MultiEntityVert.glsl",
          "assets/shaders/Entity/MultiEntityFrag.glsl")},
      renderable_entities_{std::make_unique<EntityRenderCache>()} {}

MultiEntityRender::~MultiEntityRender() = default;

size_t MultiEntityRender::GetNumEntityRendered() {
  return num_entity_rendered_;
}

void MultiEntityRender::InsertEntity(const EntityProperty& entity) {
  renderable_entities_->AddEntity(entity);
}

void MultiEntityRender::RemoveEntity(EntityUUID entityUUID) {
  renderable_entities_->RemoveEntity(entityUUID);
}

void MultiEntityRender::Clean() {
  vbo_ = std::make_unique<Buffer>(game_context_);
  ebo_ = std::make_unique<Buffer>(game_context_);
  ssbo_pos_ = std::make_unique<Buffer>(game_context_);
  ssbo_vel_ = std::make_unique<Buffer>(game_context_);
  ssbo_acc_ = std::make_unique<Buffer>(game_context_);
  vao_ = std::make_unique<VertexArray>(game_context_);
}

void MultiEntityRender::Initialize(PerformanceProfiler* pProfilerIn) {
  static constexpr int kMaxEntityCount = 10000000;

  for (int i = 0; i < game_context_.entities_list_->entity_type_list_.size();
       i++) {
    EntityModel model =
        game_context_.entities_list_->entity_type_list_[i]->render_model_;
    entity_cached_models_[i] = model;
    profiler_ = pProfilerIn;
  }

  // Initialize Buffers

  for (auto& EntityModels : entity_cached_models_) {
    Model::ModelData model = EntityModels.second.GetVertices();

    uint32_t ModelIndex = static_cast<uint32_t>(entity_vertices_.size() / 5);

    entity_element_index_[EntityModels.first] =
        entity_indices_.size();  // temp solution
    entity_element_size_[EntityModels.first] = model.indices_.size();

    for (int i = 0; i < model.indices_.size(); i++) {
      entity_indices_.emplace_back(ModelIndex + model.indices_[i]);
    }

    entity_vertices_.insert(entity_vertices_.end(), model.vertices_.begin(),
                            model.vertices_.end());
  }

  vao_->Bind();

  vbo_->SetType(GL_ARRAY_BUFFER);
  vbo_->SetUsage(GL_STATIC_DRAW);
  vbo_->InsertData(entity_vertices_.size() * sizeof(float),
                   entity_vertices_.data(), GL_STATIC_DRAW);

  ebo_->SetUsage(GL_STATIC_DRAW);
  ebo_->SetType(GL_ELEMENT_ARRAY_BUFFER);
  ebo_->InsertData(entity_indices_.size() * sizeof(uint32_t),
                   entity_indices_.data(), GL_STATIC_DRAW);

  ssbo_pos_->SetType(GL_SHADER_STORAGE_BUFFER);
  ssbo_pos_->SetUsage(GL_DYNAMIC_COPY);
  ssbo_pos_->SetMaxSize(kMaxEntityCount);
  ssbo_pos_->InitializeData();

  ssbo_vel_->SetType(GL_SHADER_STORAGE_BUFFER);
  ssbo_vel_->SetUsage(GL_DYNAMIC_COPY);
  ssbo_vel_->SetMaxSize(kMaxEntityCount);
  ssbo_vel_->InitializeData();

  ssbo_acc_->SetType(GL_SHADER_STORAGE_BUFFER);
  ssbo_acc_->SetUsage(GL_DYNAMIC_COPY);
  ssbo_acc_->SetMaxSize(kMaxEntityCount);
  ssbo_acc_->InitializeData();

  vao_->EnableAttriPtr(vbo_.get(), 0, 3, GL_FLOAT, GL_FALSE, 5, 0)
      .EnableAttriPtr(vbo_.get(), 1, 2, GL_FLOAT, GL_FALSE, 5, 3);

  position_arr_.resize(kMaxEntityCount);
  velocity_arr_.resize(kMaxEntityCount);
  acceleration_arr_.resize(kMaxEntityCount);
}

void MultiEntityRender::SetTimePastTick(double t) { time_past_tick_ = t; }

void MultiEntityRender::SetupCall() {
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDisable(GL_CULL_FACE);
}

void MultiEntityRender::Render() {
  profiler_->ProfileStart("root/render/entity");
  SetupCall();

  size_t n = 0;

  for (const auto& entityarr : renderable_entities_->entity_separated_) {
    // Fill SSBO POS data

    size_t i = 0;
    for (const auto& properties : entityarr.second) {
      position_arr_[(i * 3 + 0)] = properties.position_.x;
      position_arr_[(i * 3 + 1)] = properties.position_.y;
      position_arr_[(i * 3 + 2)] = properties.position_.z;

      velocity_arr_[(i * 3 + 0)] = properties.velocity_.x;
      velocity_arr_[(i * 3 + 1)] = properties.velocity_.y;
      velocity_arr_[(i * 3 + 2)] = properties.velocity_.z;

      acceleration_arr_[(i * 3 + 0)] = properties.acceleration_.x;
      acceleration_arr_[(i * 3 + 1)] = properties.acceleration_.y;
      acceleration_arr_[(i * 3 + 2)] = properties.acceleration_.z;

      i++;
    }

    n += entityarr.second.size();

    ssbo_pos_->InsertSubData(0, (i * 3) * sizeof(position_arr_[0]),
                             position_arr_.data());
    ssbo_vel_->InsertSubData(0, (i * 3) * sizeof(velocity_arr_[0]),
                             velocity_arr_.data());
    ssbo_acc_->InsertSubData(0, (i * 3) * sizeof(acceleration_arr_[0]),
                             acceleration_arr_.data());

    shader_->Use();
    shader_->BindTexture2D(
        0,
        game_context_.entities_list_->entity_type_list_[entityarr.first]
            ->texture_->Get(),
        "EntityTexture");

    vao_->Bind();
    ebo_->Bind();
    vbo_->Bind();
    shader_->BindBufferAsSSBO(ssbo_pos_->GetId(), 1)
        .BindBufferAsSSBO(ssbo_vel_->GetId(), 2)
        .BindBufferAsSSBO(ssbo_acc_->GetId(), 3);

    glDrawElementsInstanced(
        GL_TRIANGLES,
        static_cast<GLsizei>(entity_element_size_[entityarr.first]),
        GL_UNSIGNED_INT,
        reinterpret_cast<void*>(entity_element_index_[entityarr.first] *
                                sizeof(uint32_t)),
        static_cast<GLsizei>(i));

    shader_->UnbindBufferSSBO(3).UnbindBufferSSBO(2).UnbindBufferSSBO(1);

    vbo_->Unbind();
    ebo_->Unbind();
    vao_->Unbind();
  }
  num_entity_rendered_ = n;
  profiler_->ProfileStop("root/render/entity");
}

void MultiEntityRender::SetWindow(GLFWwindow* win) { window_ = win; }

void MultiEntityRender::Update() {
  profiler_->ProfileStart("root/update/entity");
  int width, height;

  glfwGetWindowSize(window_, &width, &height);
  glm::mat4 model = glm::mat4(1.f);

  glm::mat4 view = player_->GetCamera()->GetViewMatrix();

  int x = width;
  int y = height;
  glm::mat4 projection =
      glm::perspective(glm::radians(player_->GetCamera()->fov_),
                       static_cast<float>(x) / y, 0.1f, 1000000.0f);

  shader_->Use();
  shader_->SetMat4("view", view)
      .SetMat4("model", model)
      .SetMat4("projection", projection)
      .SetFloat("RenderDistance",
                static_cast<float>(horizontal_render_distance_ * kChunkDim))
      .SetFloat("VerticalRenderDistance",
                static_cast<float>(vertical_render_distance_ * kChunkDim))
      .SetVec3("camPos", player_->GetPosition())
      .SetFloat("TimeDelta", static_cast<float>(time_past_tick_));

  profiler_->ProfileStop("root/update/entity");
}

void MultiEntityRender::Reload() {
  // reset gpu data
  vbo_ = std::make_unique<Buffer>(game_context_);
  ebo_ = std::make_unique<Buffer>(game_context_);
  vao_ = std::make_unique<VertexArray>(game_context_);

  // reset entity models
  for (auto e : game_context_.entities_list_->entity_type_list_) {
    e->render_model_.Clear();
  }

  game_context_.entities_list_->Initialize();

  Initialize(profiler_);
}

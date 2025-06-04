// Copyright (c) 2025 Voxel-Game Author. All rights reserved.
#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

#include "Core/Typenames.h"

class Buffer;
class VertexArray;
class EntityModel;
class Shader;
class PerformanceProfiler;
class Camera;
class PlayerPOV;
class GameContext;

struct EntityRenderCache;
struct EntityProperty;
struct GLFWwindow;

class MultiEntityRender {
 public:
  int vertical_render_distance_ = 16;
  int horizontal_render_distance_ = 16;

  MultiEntityRender(GameContext& game_context, PlayerPOV*);
  ~MultiEntityRender();

  void Clean();

  size_t GetNumEntityRendered();

  void InsertEntity(const EntityProperty& entity);

  void RemoveEntity(EntityUUID EntityUUID);

  void Initialize(PerformanceProfiler* pProfilerIn);

  void SetTimePastTick(double t);

  void SetupCall();

  void Render();

  void SetWindow(GLFWwindow* win);

  void Update();

  void Reload();

 private:
  GameContext& game_context_;

  std::vector<float> entity_vertices_;
  std::vector<uint32_t> entity_indices_;

  FastHashMap<EntityTypeID, EntityModel> entity_cached_models_;
  FastHashMap<EntityTypeID, size_t> entity_element_index_;
  FastHashMap<EntityTypeID, size_t> entity_element_size_;

  std::unique_ptr<Buffer> vbo_;
  std::unique_ptr<Buffer> ebo_;
  std::unique_ptr<Buffer> ssbo_pos_, ssbo_vel_, ssbo_acc_;
  std::unique_ptr<VertexArray> vao_;

  PlayerPOV* player_;
  GLFWwindow* window_;

  std::unique_ptr<Shader> shader_;
  std::unique_ptr<EntityRenderCache> renderable_entities_;

  std::vector<float> position_arr_;
  std::vector<float> velocity_arr_;
  std::vector<float> acceleration_arr_;

  PerformanceProfiler* profiler_;

  size_t num_entity_rendered_ = 0;

  double time_past_tick_ = 0.0;
};

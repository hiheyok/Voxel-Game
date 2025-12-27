#pragma once

#include <memory>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"
#include "RenderEngine/RenderResources/RenderHandle.h"

class Buffer;
class VertexArray;
class Shader;
class GameContext;
class ECSManager;
class EntitySystems;
class EntityRegistry;
class ECSManager;
class PlayerPOV;
class Texture2DV2;
class EntityModel;
class RenderDrawElementsInstanced;

struct GLFWwindow;

class ECSEntityRender {
 public:
  ECSEntityRender(GameContext& context, ECSManager& manager, PlayerPOV* player);
  ~ECSEntityRender();

  void Initialize();
  void Render();
  void SetTimePastTick(double t);
  void SetWindow(GLFWwindow* win);

 private:
  void SetupCall();
  void InitializeBuffers();

  GameContext& context_;
  EntitySystems& systems_;
  EntityRegistry& registry_;
  PlayerPOV* player_;
  GLFWwindow* window_;

  RenderHandle<Shader> shader_;

  std::unique_ptr<RenderDrawElementsInstanced> renderer_;
  std::unique_ptr<Buffer> ssbo_pos_, ssbo_vel_, ssbo_acc_;

  // Reuse logic from MultiEntityRender for caching models
  FastHashMap<EntityType, AssetHandle<EntityModel>> entity_cached_models_;
  FastHashMap<EntityType, size_t> entity_element_index_;
  FastHashMap<EntityType, size_t> entity_element_size_;

  std::vector<float> entity_vertices_;
  std::vector<uint32_t> entity_indices_;

  // Instancing buffers
  std::vector<float> position_arr_;
  std::vector<float> velocity_arr_;
  std::vector<float> acceleration_arr_;

  double time_past_tick_ = 0.0;
  int horizontal_render_distance_ = 16;
  int vertical_render_distance_ = 16;
};

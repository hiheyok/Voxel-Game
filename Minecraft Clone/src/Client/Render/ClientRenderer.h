// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

class GameContext;
class PlayerPOV;
class IECSManager;
class TexturedFrameBuffer;
class WorldRender;
class MultiEntityRender;
class ECSEntityRender;
class ClientCache;
class PerformanceProfiler;
class Window;

struct GLFWwindow;

// Orchestrates all rendering: framebuffer management and render passes.
class ClientRenderer {
 public:
  ClientRenderer(GameContext& context, PlayerPOV* player,
                 IECSManager& ecs_manager);
  ~ClientRenderer();

  ClientRenderer(const ClientRenderer&) = delete;
  ClientRenderer& operator=(const ClientRenderer&) = delete;

  void Initialize(Window* window, ClientCache* cache,
                  PerformanceProfiler* profiler, float graphics_scale);
  void Render(Window* window);
  void Update(Window* window, float graphics_scale);
  void HandleReload();

  // Accessors needed by other components
  WorldRender& GetTerrainRender();
  MultiEntityRender& GetEntityRender();
  ECSEntityRender& GetECSEntityRender();

  void SetTimePastTick(double t);

 private:
  GameContext& context_;
  std::unique_ptr<TexturedFrameBuffer> framebuffer_;
  std::unique_ptr<WorldRender> terrain_render_;
  std::unique_ptr<MultiEntityRender> entity_render_;
  std::unique_ptr<ECSEntityRender> ecs_entity_render_;
};

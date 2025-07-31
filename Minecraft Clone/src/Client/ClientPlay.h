// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "Core/Typenames.h"
// This will handle all of the world rendering and interaction with the server
// and play controls stuff

class GameContext;
class ServerInterface;
class TexturedFrameBuffer;
class MainPlayer;
class WorldRender;
class InputManager;
class ClientLevel;
class PerformanceProfiler;
class DebugScreen;
class Window;
class MultiEntityRender;

struct WindowProperties;
struct GLFWwindow;

class ClientPlay {
 public:
  ClientPlay(GameContext& game_context, ServerInterface* interface,
             Window* window, PerformanceProfiler* profiler);
  ~ClientPlay();

  void Update(Window* window);
  void Render(Window* window);
  
  GameContext& game_context_;
  ServerInterface* interface_;
  std::unique_ptr<TexturedFrameBuffer> framebuffer_;
  std::unique_ptr<ClientLevel> client_level_;
  std::unique_ptr<MainPlayer> main_player_;
  std::unique_ptr<DebugScreen> debug_screen_;
  std::unique_ptr<MultiEntityRender> entity_render_;
  std::unique_ptr<WorldRender> terrain_render_;

  double frametime_ = 1;

 private:
  void UpdateDebugStats();
  void UpdateChunks();
  void UpdateEntities();
};

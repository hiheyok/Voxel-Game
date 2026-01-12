// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "Core/Typenames.h"
#include "RenderEngine/UI/Screens/Screen.h"
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
class Window;
class MultiEntityRender;
class ECSEntityRender;
class ClientPacketReceiver;
class ClientPacketSender;
class ClientActionQueue;
class UIManager;

struct WindowProperties;
struct GLFWwindow;

class ClientPlay {
 public:
  ClientPlay(GameContext& context, ServerInterface& interface, Window* window,
             PerformanceProfiler* profiler, UIManager& ui_manager);
  ~ClientPlay();

  void Update(Window* window);
  void Render(Window* window);
  
  Screen::TickCallback GetDebugStatsCallback();
  Screen::TickCallback GetHotbarUpdateCallback();

  GameContext& context_;
  ServerInterface& interface_;

  std::unique_ptr<TexturedFrameBuffer> framebuffer_;
  std::unique_ptr<ClientLevel> client_level_;
  std::unique_ptr<MainPlayer> main_player_;
  std::unique_ptr<MultiEntityRender> entity_render_;
  std::unique_ptr<ECSEntityRender> ecs_entity_render_;
  std::unique_ptr<WorldRender> terrain_render_;
  std::unique_ptr<ClientPacketReceiver> packet_receiver_;
  std::unique_ptr<ClientPacketSender> packet_sender_;
  std::unique_ptr<ClientActionQueue> action_queue_;

  double frametime_ = 1;
};

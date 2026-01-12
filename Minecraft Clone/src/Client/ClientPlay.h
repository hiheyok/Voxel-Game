// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "Core/Typenames.h"
#include "RenderEngine/UI/Screens/Screen.h"
// This will handle all of the world rendering and interaction with the server
// and play controls stuff

class GameContext;
class ServerInterface;
class MainPlayer;
class InputManager;
class ClientLevel;
class Window;
class ClientPacketReceiver;
class ClientPacketSender;
class ClientActionQueue;
class UIManager;
class ClientRenderer;
class DebugStatsCollector;
class ClientUICallbacks;

struct WindowProperties;
struct GLFWwindow;

class ClientPlay {
 public:
  ClientPlay(GameContext& context, ServerInterface& interface, Window* window,
             UIManager& ui_manager);
  ~ClientPlay();

  void Update(Window* window);
  void Render(Window* window);

  Screen::TickCallback GetDebugStatsCallback();
  Screen::TickCallback GetHotbarUpdateCallback();

  GameContext& context_;
  ServerInterface& interface_;

  std::unique_ptr<ClientLevel> client_level_;
  std::unique_ptr<MainPlayer> main_player_;
  std::unique_ptr<ClientRenderer> renderer_;
  std::unique_ptr<ClientPacketReceiver> packet_receiver_;
  std::unique_ptr<ClientPacketSender> packet_sender_;
  std::unique_ptr<ClientActionQueue> action_queue_;
  std::unique_ptr<DebugStatsCollector> stats_collector_;
  std::unique_ptr<ClientUICallbacks> ui_callbacks_;

  double frametime_ = 1;
};

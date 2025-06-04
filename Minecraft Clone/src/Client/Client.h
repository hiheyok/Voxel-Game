// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "RenderEngine/Window.h"

class ClientPlay;
class MainPlayer;
class WorldRender;
class TextRenderer;
class PerformanceProfiler;
class InternalInterface;
class ClientLevel;
class Server;
class GameContext;

class Client : public Window {
 public:
  Client(GameContext&);
  ~Client();

  void run();

 private:
  void Initialize();
  void GameLoop();
  void Update();
  void Cleanup();
  void Render();
  void InitializeServerCom();

  double frametime_ = 0.5;

  GameContext& game_context_;
  EntityUUID player_uuid_;
  std::unique_ptr<ClientPlay> client_play_;
  std::unique_ptr<Server> server_;
  std::unique_ptr<TextRenderer> text_render_;
  std::unique_ptr<InternalInterface> internal_interface_;
  PerformanceProfiler* profiler_;
};

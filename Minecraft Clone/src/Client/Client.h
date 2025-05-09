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

class Client : public Window {
 public:
  Client();
  ~Client();

  void run();

 private:
  void Initialize();
  void GameLoop();
  void Update();
  void Cleanup();
  void Render();
  void InitializeServerCom();
  void InitializeGameContent();

  double frametime_ = 0.5;

  EntityUUID player_uuid_;
  std::unique_ptr<ClientPlay> client_play_;
  std::unique_ptr<Server> server_;
  std::unique_ptr<InternalInterface> internal_interface_;
  std::unique_ptr<TextRenderer> text_render_;
  PerformanceProfiler* profiler_;
};

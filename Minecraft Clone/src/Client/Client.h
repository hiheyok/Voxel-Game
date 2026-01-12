// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "RenderEngine/Window.h"

class ClientPlay;
class ClientServerBridge;
class PerformanceProfiler;
class GameContext;
class UIManager;

class Client : public Window {
 public:
  explicit Client(GameContext&);
  ~Client();

  void run();

 private:
  void Initialize();
  void GameLoop();
  void Update();
  void Cleanup();
  void Render();

  double frametime_ = 0.5;

  GameContext& context_;
  std::unique_ptr<ClientServerBridge> server_bridge_;
  std::unique_ptr<ClientPlay> client_play_;
  std::unique_ptr<UIManager> ui_manager_;
  PerformanceProfiler* profiler_;
};

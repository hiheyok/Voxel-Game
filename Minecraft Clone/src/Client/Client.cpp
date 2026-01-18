// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Client.h"

#include <gl/glew.h>
#include <glfw/glfw3.h>

#include <memory>

#include "Client/ClientPlay.h"
#include "Client/Inputs/InputManager.h"
#include "Client/Network/ClientServerBridge.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/UI/Screens/DebugOverlay.h"
#include "RenderEngine/UI/UIManager.h"
#include "Utils/Clock.h"
#include "Utils/Timer/Timer.h"

using std::make_unique;

Client::Client(GameContext& context) : Window{context}, context_{context} {}

Client::~Client() = default;

void Client::Initialize() {
  context_.InitializeRenderingContext();
  DisableCursor();

  // Create UIManager first so it can be passed to ClientPlay
  ui_manager_ = make_unique<UIManager>(context_, inputs_);
  ui_manager_->Initialize();
  int hud_idx = ui_manager_->PushScreen("player_hud");
  int debug_idx = ui_manager_->PushScreen("debug_overlay");

  // Create server bridge and connect
  server_bridge_ = make_unique<ClientServerBridge>(context_);
  server_bridge_->Connect();

  // Create ClientPlay with server interface
  client_play_ = make_unique<ClientPlay>(
      context_, server_bridge_->GetInterface(), this, *ui_manager_);

  // Set up UI callbacks
  ui_manager_->SetScreenTickCallback(debug_idx,
                                     client_play_->GetDebugStatsCallback());
  ui_manager_->SetScreenTickCallback(hud_idx,
                                     client_play_->GetHotbarUpdateCallback());
}

void Client::run() {
  Initialize();
  GameLoop();
  Cleanup();
}

void Client::Cleanup() { server_bridge_->Stop(); }

void Client::Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  client_play_->Render(this);
  ui_manager_->Render();
}

void Client::GameLoop() {
  int fps = 360;

  while (!WindowCloseCheck()) {
    Timer frametime_tracker;

    Update();

    Render();

    Refresh();

    double curr = frametime_tracker.GetTimePassed_s();

    if (curr < (1.0 / fps)) {
      timerSleep((1.0 / fps) - curr);
    }

    frametime_ = frametime_tracker.GetTimePassed_s();
    client_play_->frametime_ = frametime_;
    inputs_.delta_ = frametime_;
  }
}

void Client::Update() {
  PollInputs();

  if (inputs_.CheckAction(InputAction::kDrawWireFrame)) {
    properties_.draw_solid_ = false;
  }
  if (inputs_.CheckAction(InputAction::kDrawSolids)) {
    properties_.draw_solid_ = true;
  }

  if (inputs_.CheckAction(InputAction::kExit)) {
    glfwSetWindowShouldClose(GetWindow(), true);
  }

  ui_manager_->ScreenResChanged(inputs_.GetScreenRes());
  ui_manager_->Update();

  client_play_->Update(this);

  inputs_.UpdateAllKey();
}

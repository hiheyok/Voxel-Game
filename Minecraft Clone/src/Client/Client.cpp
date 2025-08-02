// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Client.h"

#include <GLFW/glfw3.h>
#include <gl/glew.h>

#include <chrono>
#include <memory>

#include "Client/ClientPlay.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/InternalInterface.h"
#include "Core/Networking/Packet.h"
#include "Core/Options/Option.h"
#include "Level/Block/Blocks.h"
#include "Level/Chunk/Chunk.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Item/ItemTextureAtlas.h"
#include "Level/Item/Items.h"
#include "Level/Level.h"
#include "Player/MainPlayer.h"
#include "Profiler/PerformanceProfiler.h"
#include "Render/DebugScreen/DebugScreen.h"
#include "Render/WorldRender.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/GUI/TextRenderer.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/UI/UIManager.h"
#include "Server/Server.h"
#include "Utils/Clock.h"
#include "Utils/LogUtils.h"
#include "Utils/Timer/Timer.h"

Client::Client(GameContext& game_context)
    : Window{game_context},
      game_context_{game_context},
      server_{std::make_unique<Server>(game_context)},
      text_render_{std::make_unique<TextRenderer>(game_context)},
      internal_interface_{std::make_unique<InternalInterface>()},
      profiler_{new PerformanceProfiler()} {}

Client::~Client() = default;

void Client::InitializeServerCom() {
  // Start Server First
  ServerSettings settings;
  settings.gen_thread_count_ = game_context_.options_->world_gen_threads_;
  settings.light_engine_thread_count_ =
      game_context_.options_->light_engine_threads_;
  settings.horizontal_ticking_distance_ =
      game_context_.options_->horizontal_render_distance_;
  settings.vertical_ticking_distance_ =
      game_context_.options_->vertical_render_distance_;

  // Joins the server it should start receiving stuff now
  server_->StartServer(settings);
  player_uuid_ = server_->SetInternalConnection(internal_interface_.get());
  client_play_ = std::make_unique<ClientPlay>(
      game_context_, internal_interface_.get(), this, profiler_);
}

void Client::Initialize() {
  game_context_.InitializeRenderingContext();
  DisableCursor();
  InitializeServerCom();
  ui_manager_ = std::make_unique<UIManager>(game_context_);
  ui_manager_->Initialize();
  ui_manager_->PushScreen("debug_screen");

  /// Initialize game rendering context
}

void Client::run() {
  Initialize();
  GameLoop();
  Cleanup();
}

void Client::Cleanup() { server_->Stop(); }

void Client::Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  client_play_->Render(this);
  // ui_manager_->Render();
}

void Client::GameLoop() {
  int fps = 120;

  while (!WindowCloseCheck()) {
    Timer frametime_tracker;

    Update();

    Render();

    profiler_->ProfileStart("root/refresh");
    Refresh();
    profiler_->ProfileStop("root/refresh");

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

  if (inputs_.CheckAction(InputAction::kPrintProfiler)) {
    profiler_->LoadCache();
    profiler_->Print();
  }

  if (inputs_.CheckAction(InputAction::kExit)) {
    glfwSetWindowShouldClose(GetWindow(), true);
  }

  client_play_->Update(this);

  inputs_.UpdateAllKey();
}

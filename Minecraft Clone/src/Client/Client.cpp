#include "Client/Client.h"

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>

#include "Client/ClientPlay.h"
#include "Client/IO/KEY_CODE.h"
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
#include "Server/Server.h"
#include "Utils/Clock.h"
#include "Utils/LogUtils.h"
#include "Utils/Timer/Timer.h"

Client::Client()
    : server_{std::make_unique<Server>()},
      text_render_{std::make_unique<TextRenderer>()},
      internal_interface_{std::make_unique<InternalInterface>()},
      profiler_{new PerformanceProfiler()} {}

Client::~Client() = default;

void Client::InitializeServerCom() {
  // Start Server First
  ServerSettings settings;
  settings.gen_thread_count_ = g_app_options.world_gen_threads_;
  settings.light_engine_thread_count_ = g_app_options.light_engine_threads_;
  settings.horizontal_ticking_distance_ =
      g_app_options.horizontal_render_distance_;
  settings.vertical_ticking_distance_ = g_app_options.vertical_render_distance_;

  // Joins the server it should start receiving stuff now
  server_->StartServer(settings);
  player_uuid_ = server_->SetInternalConnection(internal_interface_.get());
  client_play_ =
      std::make_unique<ClientPlay>(internal_interface_.get(), this, profiler_);
}
void Client::InitializeGameContent() {
  g_blocks.Initialize();
  g_entity_list.Initialize();
  text_render_->InitializeTextRenderer(GetWindow());
  g_items.RegisterAll();
  g_item_atlas.Initialize(512 * 16 * 2, 16 * 2 * 8);
  for (auto& item : g_items.item_container_) {
    g_item_atlas.AddItem(item.second);
  }
}

void Client::Initialize() {
  DisableCursor();
  InitializeGameContent();
  InitializeServerCom();
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

  if (inputs_.CheckKey(KEY_F)) {
    properties_.draw_solid_ = false;
  }
  if (inputs_.CheckKey(KEY_G)) {
    properties_.draw_solid_ = true;
  }

  if (inputs_.CheckKeyPress(KEY_P)) {
    profiler_->LoadCache();
    profiler_->Print();
  }

  if (inputs_.CheckKey(KEY_ESCAPE)) {
    glfwSetWindowShouldClose(GetWindow(), true);
  }

  client_play_->Update(this);

  inputs_.UpdateAllKey();
}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPlay.h"

#include <memory>
#include <vector>

#include "Client/ClientPacketReceiver.h"
#include "Client/ClientPacketSender.h"
#include "Client/Inputs/InputManager.h"
#include "Client/Player/MainPlayer.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Client/Render/DebugScreen/DebugScreen.h"
#include "Client/Render/WorldRender.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Options/Option.h"
#include "Level/Entity/Mobs/Player.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "RenderEngine/EntityRender/ECSEntityRender.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/Window.h"

using std::make_unique;
using std::to_string;

ClientPlay::ClientPlay(GameContext& context, ServerInterface* interface,
                       Window* window, PerformanceProfiler* profiler)
    : context_{context},
      interface_{interface},
      framebuffer_{make_unique<TexturedFrameBuffer>(context)},
      client_level_{make_unique<ClientLevel>(context)},
      main_player_{make_unique<MainPlayer>(context, window, interface,
                                           &client_level_->cache_)},
      debug_screen_{make_unique<DebugScreen>(context)},
      entity_render_{make_unique<MultiEntityRender>(
          context, main_player_->GetPlayerPOV())},
      ecs_entity_render_{make_unique<ECSEntityRender>(
          context, client_level_->cache_.GetECSManager(),
          main_player_->GetPlayerPOV())},
      terrain_render_{
          make_unique<WorldRender>(context, main_player_->GetPlayerPOV())} {
  // Initialize packet handlers
  packet_receiver_ = make_unique<ClientPacketReceiver>(
      context, *client_level_, *terrain_render_, *entity_render_);
  packet_sender_ = make_unique<ClientPacketSender>(*main_player_);

  main_player_->SetPlayerPosition(0.0f, 50.0f, 0.0f);
  main_player_->SetPlayerRotation(-135.0f, -30.0f);

  debug_screen_->Initialize(window->GetWindow());
  debug_screen_->SetUpdateRate(200);
  entity_render_->Initialize(profiler);
  entity_render_->SetWindow(window->GetWindow());
  ecs_entity_render_->Initialize();
  ecs_entity_render_->SetWindow(window->GetWindow());

  // Send initial player position to server
  packet_sender_->SendPackets(interface);

  terrain_render_->Start(window->GetWindow(), &client_level_->cache_, profiler);
  framebuffer_->GenBuffer(
      window->GetProperties().res_x_, window->GetProperties().res_y_,
      static_cast<float>(context_.options_->graphics_scale_), GL_RGB);
}
ClientPlay::~ClientPlay() = default;

void ClientPlay::Render(Window* window) {
  framebuffer_->BindFBO();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!window->GetProperties().draw_solid_) window->RenderLines();

  // entity_render_->Render();
  terrain_render_->Render();
  entity_render_->Render();
  ecs_entity_render_->Render();

  if (!window->GetProperties().draw_solid_) window->RenderSolid();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  framebuffer_->UnbindFBO();
  framebuffer_->Render();

  main_player_->RenderGUIs();
  debug_screen_->Render();
}

void ClientPlay::Update(Window* window) {
  if (window->GetProperties().size_dirty_) {
    window->GetProperties().size_dirty_ = false;

    framebuffer_->Clear();
    framebuffer_->GenBuffer(
        window->GetProperties().res_x_, window->GetProperties().res_y_,
        static_cast<float>(context_.options_->graphics_scale_));
  }

  if (window->GetUserInputs().CheckAction(InputAction::kReload)) {
    entity_render_->Reload();
  }

  main_player_->Update(window->GetUserInputs());

  entity_render_->SetTimePastTick(interface_->time.GetTimePassed_s());
  entity_render_->Update();

  ecs_entity_render_->SetTimePastTick(interface_->time.GetTimePassed_s());

  // Process incoming packets from server
  packet_receiver_->ProcessPackets(interface_);

  // Send outgoing packets to server
  packet_sender_->SendPackets(interface_);

  UpdateDebugStats();
}

void ClientPlay::UpdateDebugStats() {
  ServerStats stats = interface_->GetServerStats();
  glm::vec3 player_pos = main_player_->GetEntityProperties().position_;
  BlockPos block_pos{player_pos.x, player_pos.y, player_pos.z};
  BlockPos rel_block_pos = block_pos.GetLocalPos();
  int sky_lvl = -1;
  int block_lvl = -1;
  if (client_level_->cache_.CheckChunk(block_pos.ToChunkPos())) {
    sky_lvl = client_level_->cache_.GetChunk(block_pos.ToChunkPos())
                  .sky_light_->GetLighting(rel_block_pos);
    block_lvl = client_level_->cache_.GetChunk(block_pos.ToChunkPos())
                    .block_light_->GetLighting(rel_block_pos);
  }

  debug_screen_->EditText(
      "Stat1",
      "VRAM Usage: " +
          to_string(static_cast<double>(
                        terrain_render_->renderer_->GetVRAMUsageFull()) /
                    1000000.0) +
          " MB");
  debug_screen_->EditText("Stat2", "XYZ: " + to_string(player_pos.x) + "/" +
                                       to_string(player_pos.y) + "/" +
                                       to_string(player_pos.z));
  debug_screen_->EditText(
      "Stat3",
      "Velocity XYZ: " +
          to_string(main_player_->GetEntityProperties().velocity_.x) + "/" +
          to_string(main_player_->GetEntityProperties().velocity_.y) + "/" +
          to_string(main_player_->GetEntityProperties().velocity_.z));
  debug_screen_->EditText(
      "Stat4",
      "VRAM Fragmentation Rate: " +
          to_string(terrain_render_->renderer_->GetFragmentationRate() * 100) +
          "%");
  debug_screen_->EditText("Stat5", "FPS: " + to_string(1.0 / frametime_));

  debug_screen_->EditText(
      "Stat6", "Mesh Stats (ms) Total/S0/S1/S2: " +
                   to_string(terrain_render_->build_time_ / 1000.f) + "/" +
                   to_string(100.0 * terrain_render_->build_stage_0_ /
                             terrain_render_->build_time_) +
                   "/" +
                   to_string(100.0 * terrain_render_->build_stage_1_ /
                             terrain_render_->build_time_) +
                   "/" +
                   to_string(terrain_render_->build_time_ /
                             terrain_render_->amount_of_mesh_generated_) +
                   "/" +
                   to_string((double)terrain_render_->set_face_calls_clocks_ /
                             terrain_render_->set_face_calls_));

  debug_screen_->EditText(
      "Stat7",
      "Mesh Engine Queued: " + to_string(terrain_render_->GetQueuedSize()));
  debug_screen_->EditText("Stat8", "Light Level: " + to_string(sky_lvl) +
                                       " : " + to_string(block_lvl));
  debug_screen_->EditText("Stat9",
                          "Server Tick (MSPT): " + to_string(stats.mspt_));
  debug_screen_->EditText(
      "Stat10",
      "Light Engine Queue: " + to_string(stats.light_stats_.queue_size_) +
          " | Update time: " +
          to_string(stats.light_stats_.average_light_update_time_));
  debug_screen_->Update();
}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPlay.h"

#include <glm/vec3.hpp>
#include <memory>
#include <string>

#include "Client/ClientActionQueue.h"
#include "Client/ClientPacketReceiver.h"
#include "Client/ClientPacketSender.h"
#include "Client/Inputs/InputManager.h"
#include "Client/Inputs/MouseInputs.h"
#include "Client/Player/MainPlayer.h"
#include "Level/Entity/Mobs/Player.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Client/Render/WorldRender.h"
#include "ClientLevel/ClientCache.h"
#include "ClientLevel/ClientLevel.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Options/Option.h"
#include "Core/Position/PositionTypes.h"
#include "Core/Stats/ServerStats.h"
#include "RenderEngine/ChunkRender/TerrainRenderer.h"
#include "RenderEngine/EntityRender/ECSEntityRender.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/UI/Screens/DebugOverlay.h"
#include "RenderEngine/UI/Screens/PlayerHud.h"
#include "RenderEngine/UI/UIManager.h"
#include "RenderEngine/Window.h"
#include "Level/Item/ItemStack.h"

using glm::vec3;
using std::make_unique;
using std::to_string;

ClientPlay::ClientPlay(GameContext& context, ServerInterface& interface,
                       Window* window, PerformanceProfiler* profiler,
                       UIManager& ui_manager)
    : context_{context},
      interface_{interface},
      framebuffer_{make_unique<TexturedFrameBuffer>(context)},
      client_level_{make_unique<ClientLevel>(context)},
      main_player_{
          make_unique<MainPlayer>(context, client_level_->cache_, ui_manager)},
      entity_render_{make_unique<MultiEntityRender>(
          context, main_player_->GetPlayerPOV())},
      ecs_entity_render_{make_unique<ECSEntityRender>(
          context, client_level_->cache_.GetECSManager(),
          main_player_->GetPlayerPOV())},
      terrain_render_{
          make_unique<WorldRender>(context, main_player_->GetPlayerPOV())},
      action_queue_{make_unique<ClientActionQueue>()} {
  // Initialize packet handlers
  packet_receiver_ = make_unique<ClientPacketReceiver>(
      context, *client_level_, *terrain_render_, *entity_render_);
  packet_sender_ = make_unique<ClientPacketSender>(interface, *main_player_);

  main_player_->SetPlayerPosition(0.0f, 50.0f, 0.0f);
  main_player_->SetPlayerRotation(-135.0f, -30.0f);

  entity_render_->Initialize(profiler);
  entity_render_->SetWindow(window->GetWindow());
  ecs_entity_render_->Initialize();
  ecs_entity_render_->SetWindow(window->GetWindow());

  // Send initial player position to server
  packet_sender_->SendPackets();

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

  //   debug_screen_->Render();
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

  // Update main player with action queue
  main_player_->Update(window->GetUserInputs(), *action_queue_);

  entity_render_->SetTimePastTick(interface_.time.GetTimePassed_s());
  entity_render_->Update();

  ecs_entity_render_->SetTimePastTick(interface_.time.GetTimePassed_s());

  // Process incoming packets from server
  packet_receiver_->ProcessPackets(interface_);

  // Process action queue and send outgoing packets to server
  packet_sender_->ProcessQueue(*action_queue_);
  action_queue_->Clear();

  // Send player movement (legacy, still needed for position sync)
  packet_sender_->SendPackets();

  terrain_render_->Update();
}

Screen::TickCallback ClientPlay::GetDebugStatsCallback() {
  return [this](Screen* screen) {
    DebugOverlay* debug_screen = static_cast<DebugOverlay*>(screen);
    int duration = debug_screen->timer.GetTimePassed_ms();

    if (duration <= debug_screen->update_rate_) {
      return;
    }

    debug_screen->timer.Set();

    ServerStats stats = interface_.GetServerStats();
    glm::vec3 player_pos = main_player_->GetEntityProperties().position_;
    BlockPos block_pos{player_pos.x, player_pos.y, player_pos.z};
    ChunkPos chunk_pos = block_pos.ToChunkPos();
    BlockPos rel_block_pos = block_pos.GetLocalPos();

    int sky_lvl = -1;
    int block_lvl = -1;
    if (client_level_->cache_.CheckChunk(chunk_pos)) {
      sky_lvl =
          client_level_->cache_.GetChunk(chunk_pos).sky_light_->GetLighting(
              rel_block_pos);
      block_lvl =
          client_level_->cache_.GetChunk(chunk_pos).block_light_->GetLighting(
              rel_block_pos);
    }

    float vram_usage =
        static_cast<float>(terrain_render_->renderer_->GetVRAMUsageFull()) /
        1000000;

    vec3 velocity = main_player_->GetEntityProperties().velocity_;

    debug_screen->Edit(0, "VRAM Usage {} MB", vram_usage);
    debug_screen->Edit(1, "Position: x={}, y={}, z={}", player_pos.x,
                       player_pos.y, player_pos.z);
    debug_screen->Edit(2, "Velocity: x={}, y={}, z={}", velocity.x, velocity.y,
                       velocity.z);

    float vram_frag_rate =
        terrain_render_->renderer_->GetFragmentationRate() * 100;
    debug_screen->Edit(3, "VRAM Fragmentation Rate: {}%", vram_frag_rate);

    float fps = 1.0 / frametime_;
    debug_screen->Edit(4, "FPS: {}", fps);

    float mesh_ms = terrain_render_->build_time_ / 1000;
    float s0 =
        terrain_render_->build_stage_0_ / terrain_render_->build_time_ * 100;
    float s1 =
        terrain_render_->build_stage_1_ / terrain_render_->build_time_ * 100;
    float avg = terrain_render_->build_time_ /
                terrain_render_->amount_of_mesh_generated_;

    debug_screen->Edit(5, "Mesh Stats (ms) Total={} S0={} S1={} Avg={} us",
                       mesh_ms, s0, s1, avg);

    debug_screen->Edit(6, "Mesh Engine Queued: {}",
                       terrain_render_->GetQueuedSize());

    debug_screen->Edit(7, "Light Level: Sky={}, Block={}", sky_lvl, block_lvl);
    debug_screen->Edit(8, "Server Tick (MSPT): {}", stats.mspt_);
    debug_screen->Edit(9, "Light Engine Queue: {} | Update time: {}",
                       stats.light_stats_.queue_size_,
                       stats.light_stats_.average_light_update_time_);
  };
}

Screen::TickCallback ClientPlay::GetHotbarUpdateCallback() {
  return [this](Screen* screen) {
    PlayerHud* player_hud = static_cast<PlayerHud*>(screen);
    auto& inventory = main_player_->player_->entity_inventory_;

    // Update slot contents
    for (int i = 0; i < 9; i++) {
      ItemStack item = inventory.GetItem(i);
      if (item.IsInitialized()) {
        player_hud->UpdateSlot(i, item.item_);
      }
    }

    // Update selection indicator position
    player_hud->UpdateSelectedSlot(inventory.right_hand_slot_);
  };
}

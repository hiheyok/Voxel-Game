// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/ClientPlay.h"

#include <glm/vec3.hpp>
#include <memory>
#include <string>

#include "Client/ClientActionQueue.h"
#include "Client/ClientPacketReceiver.h"
#include "Client/ClientPacketSender.h"
#include "Client/Inputs/InputManager.h"
#include "Client/Player/MainPlayer.h"
#include "Client/Profiler/PerformanceProfiler.h"
#include "Client/Render/ClientRenderer.h"
#include "Client/Render/WorldRender.h"
#include "Client/Stats/DebugStatsCollector.h"
#include "Client/UI/ClientUICallbacks.h"
#include "Client/ClientLevel/ClientCache.h"
#include "Client/ClientLevel/ClientLevel.h"
#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ServerInterface.h"
#include "Core/Options/Option.h"
#include "RenderEngine/EntityRender/MultiEntityRender.h"
#include "RenderEngine/UI/UIManager.h"
#include "RenderEngine/Window.h"

using std::make_unique;

ClientPlay::ClientPlay(GameContext& context, ServerInterface& interface,
                       Window* window, PerformanceProfiler* profiler,
                       UIManager& ui_manager)
    : context_{context},
      interface_{interface},
      client_level_{make_unique<ClientLevel>(context)},
      main_player_{
          make_unique<MainPlayer>(context, client_level_->cache_, ui_manager)},
      renderer_{make_unique<ClientRenderer>(context, main_player_->GetPlayerPOV(),
                                            client_level_->cache_.GetECSManager())},
      action_queue_{make_unique<ClientActionQueue>()} {
  // Initialize packet handlers
  packet_receiver_ = make_unique<ClientPacketReceiver>(
      context, *client_level_, renderer_->GetTerrainRender(),
      renderer_->GetEntityRender());
  packet_sender_ = make_unique<ClientPacketSender>(interface, *main_player_);

  main_player_->SetPlayerPosition(0.0f, 50.0f, 0.0f);
  main_player_->SetPlayerRotation(-135.0f, -30.0f);

  // Initialize renderer
  renderer_->Initialize(window, &client_level_->cache_, profiler,
                        static_cast<float>(context_.options_->graphics_scale_));

  // Send initial player position to server
  packet_sender_->SendPackets();

  // Create stats collector and UI callbacks
  stats_collector_ = make_unique<DebugStatsCollector>(
      interface_, *main_player_, *renderer_, *client_level_);
  ui_callbacks_ =
      make_unique<ClientUICallbacks>(*stats_collector_, *main_player_, frametime_);
}

ClientPlay::~ClientPlay() = default;

void ClientPlay::Render(Window* window) { renderer_->Render(window); }

void ClientPlay::Update(Window* window) {
  // Handle window resize and renderer updates
  renderer_->Update(window,
                    static_cast<float>(context_.options_->graphics_scale_));

  if (window->GetUserInputs().CheckAction(InputAction::kReload)) {
    renderer_->HandleReload();
  }

  // Update main player with action queue
  main_player_->Update(window->GetUserInputs(), *action_queue_);

  renderer_->SetTimePastTick(interface_.time.GetTimePassed_s());

  // Process incoming packets from server
  packet_receiver_->ProcessPackets(interface_);

  // Process action queue and send outgoing packets to server
  packet_sender_->ProcessQueue(*action_queue_);
  action_queue_->Clear();

  // Send player movement (legacy, still needed for position sync)
  packet_sender_->SendPackets();
}

Screen::TickCallback ClientPlay::GetDebugStatsCallback() {
  return ui_callbacks_->GetDebugStatsCallback();
}

Screen::TickCallback ClientPlay::GetHotbarUpdateCallback() {
  return ui_callbacks_->GetHotbarUpdateCallback();
}

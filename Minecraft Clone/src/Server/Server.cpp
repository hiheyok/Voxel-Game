// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Server/Server.h"

#include <memory>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/ClientInterface.h"
#include "Core/Interfaces/InternalInterface.h"
#include "Core/Typenames.h"
#include "Level/Dimension/Dimension.h"
#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/ServerECSManager.h"
#include "Level/Entity/Mobs/Player.h"
#include "Level/Level.h"
#include "Level/World/WorldUpdater.h"
#include "Server/ServerPacketReceiver.h"
#include "Server/ServerPacketSender.h"
#include "Utils/Clock.h"
#include "Utils/LogUtils.h"
#include "Utils/Timer/Timer.h"

using std::make_unique;
using std::move;
using std::unique_ptr;

Server::Server(GameContext& context) : context_{context} {}
Server::~Server() {
  if (!stop_) {
    Stop();
  }
}

void Server::StartServer(ServerSettings serverSettings) {
  level_ = make_unique<Level>(context_);
  level_->Start();
  time_ = make_unique<Timer>();
  stop_ = false;
  settings_ = make_unique<ServerSettings>(serverSettings);
  packet_receiver_ = make_unique<ServerPacketReceiver>(*level_, context_);
  packet_sender_ = make_unique<ServerPacketSender>(*level_, mspt_);
  main_server_loop_ = std::thread(&Server::Loop, this);
}

void Server::Stop() {
  stop_ = true;
  main_server_loop_.join();
  level_->Stop();
}

void Server::Loop() {
  LOG_DEBUG("Started main server loop");
  while (!stop_) {
    time_->Set();
    // Process Client -> Server events
    packet_receiver_->ProcessPackets(client_interface_);
    Load();
    Tick();
    packet_sender_->SendPackets(client_interface_);
    ResetState();

    mspt_ = time_->GetTimePassed_ms();
    double time_left = (1.0 / settings_->tick_rate_) - mspt_ / 1000.0;

    if (time_left > 0) {
      timerSleep(time_left);
    }

    mspt_ = time_->GetTimePassed_ms();
  }
  LOG_DEBUG("Shutting down main server loop");
}

void Server::Tick() {
  level_->main_world_->Tick();
  level_->main_world_->EventTick();
  level_->UpdateDimensions();
}

void Server::SendEvent(const Event& eventIn) {
  level_->main_world_->event_manager_.AddEvent(eventIn);
}

EntityUUID Server::SetInternalConnection(InternalInterface* interface) {
  client_interface_ = static_cast<ClientInterface*>(interface);
  unique_ptr<Player> player = make_unique<Player>();
  EntityUUID uuid =
      level_->main_world_->world_updater_->SetEntity(move(player));
  interface->SetPlayerUUID(uuid);
  return uuid;
}

void Server::Load() { level_->main_world_->world_updater_->Load(); }

void Server::ResetState() {
  level_->main_world_->ResetState();

  // Clear ECS registry pending removals after network sync has sent them
  auto& server_ecs = static_cast<ServerECSManager&>(
      level_->main_world_->world_->GetECSManager());
  server_ecs.GetRegistry().ResetState();
}

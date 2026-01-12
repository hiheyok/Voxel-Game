// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Network/ClientServerBridge.h"

#include <memory>

#include "Core/GameContext/GameContext.h"
#include "Core/Interfaces/InternalInterface.h"
#include "Core/Options/Option.h"
#include "Level/Level.h"
#include "Server/Server.h"

using std::make_unique;

ClientServerBridge::ClientServerBridge(GameContext& context)
    : context_{context},
      internal_interface_{make_unique<InternalInterface>()} {}

ClientServerBridge::~ClientServerBridge() = default;

void ClientServerBridge::Connect() {
  // Configure server settings from options
  ServerSettings settings;
  settings.gen_thread_count_ = context_.options_->world_gen_threads_;
  settings.light_engine_thread_count_ =
      context_.options_->light_engine_threads_;
  settings.horizontal_ticking_distance_ =
      context_.options_->horizontal_render_distance_;
  settings.vertical_ticking_distance_ =
      context_.options_->vertical_render_distance_;

  // Create and start server
  server_ = make_unique<Server>(context_);
  server_->StartServer(settings);

  // Set up internal connection and get player UUID
  player_uuid_ = server_->SetInternalConnection(internal_interface_.get());
}

void ClientServerBridge::Stop() {
  if (server_) {
    server_->Stop();
  }
}

ServerInterface& ClientServerBridge::GetInterface() {
  return *internal_interface_;
}

EntityUUID ClientServerBridge::GetPlayerUUID() const { return player_uuid_; }

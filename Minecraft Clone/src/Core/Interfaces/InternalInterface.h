// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <string>
#include <iostream>

#include "ClientInterface.h"
#include "Core/DataStructure/ConcurrentQueue.h"
#include "ServerInterface.h"
#include "Utils/LogUtils.h"

class InternalInterface : public ServerInterface, public ClientInterface {
 public:
  InternalInterface() {}

  // Client -> Server
  void SendPlayerAction(const Packet::PlayerAction& action) override {
    int attempts = 0;
    while (!player_action_queue_.enqueue(action) &&
           attempts++ < kQueueAttempts) {
    }
  }

  bool IsConnected() const override { return is_connected_; }

  void Connect(const std::string& address, int port) override {
    throw std::runtime_error(
        "InternalServerInterface::Connect - This function isn't active!");
    (void)address;
    (void)port;
  }

  void Disconnect() {}

  void Update() {}

  // Server -> Client

  void SendBlockUpdate(const Packet::BlockUpdate& update) override {
    int attempts = 0;
    while (!block_update_queue_.enqueue(update) &&
           attempts++ < kQueueAttempts) {
    }
  }

  void SendEntityUpdate(const Packet::EntityUpdate& update) override {
    int attempts = 0;
    while (!entity_update_queue_.enqueue(update) &&
           attempts++ < kQueueAttempts) {
    }
  }

  void SendChunkUpdates(const Packet::ChunkUpdateData& update) override {
    int attempts = 0;
    while (!chunk_update_queue_.enqueue(update) &&
           attempts++ < kQueueAttempts) {
    }
  }

  void SendServerStats(const ServerStats& stats) override {
    server_stats_ = stats;
  }

  void SendTimeLastTick() override { time.Set(); }

 private:
  static constexpr int kQueueAttempts = 16;
  bool is_connected_ = false;
};

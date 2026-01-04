// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "Core/Typenames.h"

class ClientInterface;
class Level;
class GameContext;

// Handles incoming packets from clients
class ServerPacketReceiver {
 public:
  explicit ServerPacketReceiver(Level& level, GameContext& context);
  ~ServerPacketReceiver() = default;

  // Process all incoming packets from the client
  void ProcessPackets(ClientInterface* client);

 private:
  void ProcessPlayerPackets(ClientInterface* client);

  Level& level_;
  GameContext& context_;
};

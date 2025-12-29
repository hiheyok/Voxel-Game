// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "Core/Typenames.h"

class ClientInterface;
class Level;
class GameContext;

// Handles incoming packets from clients
class PacketReceiver {
 public:
  explicit PacketReceiver(Level& level, GameContext& context);
  ~PacketReceiver() = default;

  // Process all incoming packets from the client
  void ProcessPackets(ClientInterface* client);

 private:
  void ProcessPlayerPackets(ClientInterface* client);

  Level& level_;
  GameContext& context_;
};

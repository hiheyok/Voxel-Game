// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Core/Typenames.h"

class ClientInterface;
class Level;

struct ServerStats;

// Handles outgoing packets to clients
class ServerPacketSender {
 public:
  explicit ServerPacketSender(Level& level, const double& mspt);
  ~ServerPacketSender() = default;

  // Send all outgoing packets to the client
  void SendPackets(ClientInterface* client);

 private:
  void SendEntityUpdatePacket(ClientInterface* client);
  void SendChunkUpdatePacket(ClientInterface* client);
  void SendBlockUpdatePacket(ClientInterface* client);
  void SendECSUpdatePacket(ClientInterface* client);
  void SendServerStats(ClientInterface* client);

  Level& level_;
  const double& mspt_;
};

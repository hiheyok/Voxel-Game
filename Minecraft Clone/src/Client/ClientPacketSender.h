// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

class ServerInterface;
class MainPlayer;

// Handles outgoing packets to the server (Client -> Server)
class ClientPacketSender {
 public:
  explicit ClientPacketSender(MainPlayer& main_player);
  ~ClientPacketSender() = default;

  // Send all outgoing packets to the server
  void SendPackets(ServerInterface* server);

 private:
  void SendPlayerMovement(ServerInterface* server);

  MainPlayer& main_player_;
};

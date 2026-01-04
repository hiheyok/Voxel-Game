// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Client/IClientActionHandler.h"

class ServerInterface;
class MainPlayer;
class ClientActionQueue;

// Handles outgoing packets to the server (Client -> Server)
// Implements IClientActionHandler to process queued actions.
class ClientPacketSender : public IClientActionHandler {
 public:
  explicit ClientPacketSender(ServerInterface& server, MainPlayer& main_player);
  ~ClientPacketSender() override = default;

  // Process all queued actions and send them to the server
  void ProcessQueue(const ClientActionQueue& queue);

  // Legacy method - sends player movement directly
  void SendPackets();

  // IClientActionHandler implementation
  void HandleBlockDestroy(BlockPos pos) override;
  void HandleBlockPlace(BlockPos pos, BlockID block) override;
  void HandlePlayerMove(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc) override;

 private:
  void SendPlayerMovement();

  ServerInterface& server_;
  MainPlayer& main_player_;
};

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "Core/Typenames.h"

class ServerInterface;
class ClientLevel;
class WorldRender;
class MultiEntityRender;
class GameContext;

// Handles incoming packets from the server (Server -> Client)
class ClientPacketReceiver {
 public:
  explicit ClientPacketReceiver(GameContext& context, ClientLevel& client_level,
                                 WorldRender& terrain_render,
                                 MultiEntityRender& entity_render);
  ~ClientPacketReceiver() = default;

  // Process all incoming packets from the server
  // Returns the set of chunk positions that need rendering updates
  void ProcessPackets(ServerInterface& server);

  // Get chunk positions that need rendering updates after processing
  std::vector<ChunkPos> GetChunksToUpdate() const;

  void ClearChunksToUpdate() { chunks_to_update_.clear(); }

 private:
  void ProcessChunkUpdates(ServerInterface& server);
  void ProcessBlockUpdates(ServerInterface& server);
  void ProcessEntityUpdates(ServerInterface& server);
  void ProcessECSUpdates(ServerInterface& server);

  GameContext& context_;
  ClientLevel& client_level_;
  WorldRender& terrain_render_;
  MultiEntityRender& entity_render_;

  FastHashSet<ChunkPos> chunks_to_update_;
};

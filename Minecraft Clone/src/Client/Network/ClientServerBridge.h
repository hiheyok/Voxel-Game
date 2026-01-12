// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>

#include "Core/Typenames.h"

class GameContext;
class Server;
class InternalInterface;
class ServerInterface;

struct ServerSettings;

// Encapsulates server creation and client-server connection setup.
class ClientServerBridge {
 public:
  explicit ClientServerBridge(GameContext& context);
  ~ClientServerBridge();

  ClientServerBridge(const ClientServerBridge&) = delete;
  ClientServerBridge& operator=(const ClientServerBridge&) = delete;

  // Creates and starts the server, establishes internal connection.
  void Connect();

  // Stops the server.
  void Stop();

  // Returns the server interface for packet communication.
  ServerInterface& GetInterface();

  // Returns the player UUID assigned by the server.
  EntityUUID GetPlayerUUID() const;

 private:
  GameContext& context_;
  std::unique_ptr<Server> server_;
  std::unique_ptr<InternalInterface> internal_interface_;
  EntityUUID player_uuid_;
};

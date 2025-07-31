// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"

class Player;
class InputManager;
class ServerInterface;
class ClientCache;
class GameContext;

struct Ray;

class WorldInteraction {
 public:
  explicit WorldInteraction(GameContext&);
  ~WorldInteraction();

  void Interact(Player* player, const InputManager& Inputs,
                ServerInterface* interface, ClientCache* cache);

 private:
  BlockID GetBlock(Ray ray, ClientCache* cache);
  void BreakBlock(Ray ray, ServerInterface* interface, ClientCache* cache);
  void PlaceBlock(Ray ray, BlockID block, ServerInterface* interface,
                  ClientCache* cache);
  GameContext& game_context_;
};

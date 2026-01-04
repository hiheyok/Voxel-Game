// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"

class Player;
class InputManager;
class ClientActionQueue;
class ClientCache;
class GameContext;

struct Ray;

class WorldInteraction {
 public:
  explicit WorldInteraction(GameContext&);
  ~WorldInteraction();

  void Interact(Player& player, const InputManager& Inputs,
                ClientActionQueue& action_queue, ClientCache& cache);

 private:
  BlockID GetBlock(Ray ray, ClientCache& cache);
  void BreakBlock(Ray ray, ClientActionQueue& action_queue, ClientCache& cache);
  void PlaceBlock(Ray ray, BlockID block, ClientActionQueue& action_queue,
                  ClientCache& cache);
  GameContext& context_;
};

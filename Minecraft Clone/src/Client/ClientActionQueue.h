// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <variant>
#include <vector>

#include "Core/Typenames.h"

// Action types that can be queued
struct BlockDestroyAction {
  BlockPos pos;
};

struct BlockPlaceAction {
  BlockPos pos;
  BlockID block;
};

struct PlayerMoveAction {
  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec3 acc;
};

// Variant holding any client action
using ClientAction =
    std::variant<BlockDestroyAction, BlockPlaceAction, PlayerMoveAction>;

// Queue to collect actions during a frame.
// Decouples action generation from packet sending.
class ClientActionQueue {
 public:
  void QueueBlockDestroy(BlockPos pos);
  void QueueBlockPlace(BlockPos pos, BlockID block);
  void QueuePlayerMove(glm::vec3 pos, glm::vec3 vel, glm::vec3 acc);

  const std::vector<ClientAction>& GetActions() const;
  void Clear();

 private:
  std::vector<ClientAction> actions_;
};

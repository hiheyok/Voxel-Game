// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "Core/Typenames.h"

// Interface for handling client actions.
// Allows swapping implementations (e.g., for testing/mock).
class IClientActionHandler {
 public:
  virtual ~IClientActionHandler() = default;

  // Block interactions
  virtual void HandleBlockDestroy(BlockPos pos) = 0;
  virtual void HandleBlockPlace(BlockPos pos, BlockID block) = 0;

  // Player movement
  virtual void HandlePlayerMove(glm::vec3 pos, glm::vec3 vel,
                                glm::vec3 acc) = 0;
};

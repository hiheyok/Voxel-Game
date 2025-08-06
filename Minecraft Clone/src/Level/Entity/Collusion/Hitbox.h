// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <glm/vec3.hpp>

struct AABB {
  glm::vec3 size_ = {0.0f, 0.0f, 0.0f};
  glm::vec3 offset_ = {0.0f, 0.0f, 0.0f};
};

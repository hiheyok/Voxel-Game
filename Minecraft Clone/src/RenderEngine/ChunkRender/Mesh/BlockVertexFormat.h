// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include <cstdint>
#include <glm/vec3.hpp>

struct BlockVertexFormat {
  // 32 bit float for each position, might change to 16 bit
  glm::vec3 pos_{0.0f};
  uint32_t rgba_ = 0;  // Each channel is 8 bit r, g, b, a
  float u_ = 0.0f;
  float v_ = 0.0f;
  int light_;
};

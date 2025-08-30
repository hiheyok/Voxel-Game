// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include <cstdint>
#include <glm/vec3.hpp>

struct BlockVertexFormat {
  void Set(float x, float y, float z, int r, int g, int b, int a, float u,
           float v, int sky_light, int block_light) noexcept;
  // from 0 to 16
  void SetPos(float x, float y, float z) noexcept;
  // 8 bit coloring
  void SetColor(int r, int g, int b, int a) noexcept;
  void SetUV(float u, float v) noexcept;
  void SetLight(int sky_light, int block_light) noexcept;

  glm::vec3 pos_{0.0f, 0.0f, 0.0f};  // 32 bit float for each position, might change to 16 bit
  uint32_t color_ = 0;  // Each channel is 8 bit r, g, b, a
  float u_ = 0.0f;
  float v_ = 0.0f;
  int light_;
};

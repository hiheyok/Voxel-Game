// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include <cstdint>

struct BlockVertexPacked {
  // from 0 to 16
  void SetPos(float x, float y, float z) noexcept;
  // 8 bit coloring
  void SetColor(int r, int g, int b, int a) noexcept;
  void SetUV(int index) noexcept;
  void SetLight(int sky_light, int block_light) noexcept;

  float x_ = 0, y_ = 0, z_ = 0;
  uint32_t color_ = 0;
  uint32_t uv_light_ = 0;
};

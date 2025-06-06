// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include <cstdint>

struct BlockVertexFormat {
  void Set(float x, float y, float z, int r, int g, int b, int a, int index,
           int u, int v, int sky_light, int block_light) noexcept;
  // from 0 to 16
  void SetPos(float x, float y, float z) noexcept;
  // 8 bit coloring
  void SetColor(int r, int g, int b, int a) noexcept;
  void SetUV(int index, int u, int v) noexcept;
  void SetLight(int sky_light, int block_light) noexcept;

  float x_ = 0, y_ = 0, z_ = 0; // 32 bit float for each position, might change to 16 bit float later
  uint32_t color_ = 0; // Each channel is 8 bit r, g, b, a
  uint32_t uv_light_ = 0;
};

/*
uv_light_
- Bit (0 - 14): TextureID
- Bit (14 - 19): u
- Bit (19 - 24): v
- Bit (24 - 28): sky_light
- Bit (28 - 32): block_light
*/

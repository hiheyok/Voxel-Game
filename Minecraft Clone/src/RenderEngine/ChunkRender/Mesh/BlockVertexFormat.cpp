// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"

#include <cstdint>

void BlockVertexFormat::Set(float x, float y, float z, int r, int g, int b,
                            int a, int index, int u, int v, int sky_light,
                            int block_light) noexcept {
  SetPos(x, y, z);
  SetColor(r, g, b, a);
  SetUV(index, u, v);
  SetLight(sky_light, block_light);
}

void BlockVertexFormat::SetPos(float x, float y, float z) noexcept {
  pos_[0] = x;
  pos_[1] = y;
  pos_[2] = z;
}

void BlockVertexFormat::SetColor(int r, int g, int b, int a) noexcept {
  color_ = 0;
  color_ |= (r << 0);
  color_ |= (g << 8);
  color_ |= (b << 16);
  color_ |= (a << 24);
}

void BlockVertexFormat::SetUV(int index, int u, int v) noexcept { // 5 bits for UV coordinates
  uv_light_ &= (static_cast<uint32_t>(-1) << 24);  // clears the first 24 bits;
  uv_light_ |= index;
  uv_light_ |= u << 14;
  uv_light_ |= v << 19;
}

void BlockVertexFormat::SetLight(int sky_light, int block_light) noexcept {
  uv_light_ &= (static_cast<uint32_t>(-1) >> 8);
  uv_light_ |= (sky_light << 24) | (block_light << 28);
}
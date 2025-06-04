// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/BlockVertexPacked.h"

#include <cstdint>

void BlockVertexPacked::SetPos(float x, float y, float z) noexcept {
  x_ = x;
  y_ = y;
  z_ = z;
}

void BlockVertexPacked::SetColor(int r, int g, int b, int a) noexcept {
  color_ = 0;
  color_ |= (r << 0);
  color_ |= (g << 8);
  color_ |= (b << 16);
  color_ |= (a << 24);
}

void BlockVertexPacked::SetUV(int index) noexcept {
  uv_light_ &= (static_cast<uint32_t>(-1) << 24); // clears the first 24 bits;
  uv_light_ |= index;
}

void BlockVertexPacked::SetLight(int sky_light, int block_light) noexcept {
  uv_light_ &= (static_cast<uint32_t>(-1) >> 8);
  uv_light_ |= (sky_light << 24) | (block_light << 28);
}
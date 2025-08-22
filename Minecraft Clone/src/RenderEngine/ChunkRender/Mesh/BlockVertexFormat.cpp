// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/Mesh/BlockVertexFormat.h"

#include <cstdint>

void BlockVertexFormat::Set(float x, float y, float z, int r, int g, int b,
                            int a, float u, float v, int sky_light,
                            int block_light) noexcept {
  SetPos(x, y, z);
  SetColor(r, g, b, a);
  SetUV(u, v);
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

void BlockVertexFormat::SetUV(float u, float v) noexcept {
  u_ = u;
  v_ = v;
}

void BlockVertexFormat::SetLight(int sky_light, int block_light) noexcept {
  light_ = 0;
  light_ |= sky_light;
  light_ |= block_light << 4;
}
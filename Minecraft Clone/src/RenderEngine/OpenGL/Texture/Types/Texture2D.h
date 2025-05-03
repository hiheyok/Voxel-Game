// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "RenderEngine/OpenGL/Texture/Texture.h"

class Texture2D : public Texture {
 public:
  explicit Texture2D(const RawTextureData& data);
  Texture2D();

  bool Load(const RawTextureData& data);
};

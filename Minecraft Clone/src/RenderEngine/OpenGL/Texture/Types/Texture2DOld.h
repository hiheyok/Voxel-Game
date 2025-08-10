// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "RenderEngine/OpenGL/Texture/TextureOld.h"

class Texture2D : public Texture {
 public:
  Texture2D(GameContext&,const RawTextureData& data);
  explicit Texture2D(GameContext&);
  ~Texture2D();

  bool Load(const RawTextureData& data);
};

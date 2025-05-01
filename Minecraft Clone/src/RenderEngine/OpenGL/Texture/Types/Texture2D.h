#pragma once
#include "RenderEngine/OpenGL/Texture/Texture.h"

class Texture2D : public Texture {
 public:
  Texture2D(const RawTextureData& data);
  Texture2D();

  bool Load(const RawTextureData& data);
};

#pragma once

#include "RenderEngine/RenderResources/Types/Texture/Texture.h"

class Texture2DBase : public TextureV2 {
 public:
  ~Texture2DBase();

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;

 protected:
  Texture2DBase(GameContext& context, const std::string& key);

  int width_;
  int height_;
};

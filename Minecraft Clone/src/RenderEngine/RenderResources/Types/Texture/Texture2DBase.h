#pragma once

#include "RenderEngine/RenderResources/Types/Texture/Texture.h"

class Texture2DBase : public Texture {
 public:
  ~Texture2DBase();

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;

 protected:
  Texture2DBase(GameContext& context, const std::string& key);
  Texture2DBase(GameContext& context, const std::string& key, int width,
                int height);

  int width_;
  int height_;
};

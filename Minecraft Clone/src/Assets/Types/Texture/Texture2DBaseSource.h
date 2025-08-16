#pragma once

#include "Assets/Types/Texture/TextureSource.h"

class Texture2DBaseSource : public TextureSource {
 public:
  ~Texture2DBaseSource();

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;

 protected:
  Texture2DBaseSource(GameContext& context, const std::string& asset_key);

  int width_ = 0;
  int height_ = 0;
};

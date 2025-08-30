#pragma once

#include "RenderEngine/API/TextureBase.h"

class GameContext;

namespace framebuffer {

class Attachment {
 public:
  Attachment(GameContext&, int width, int height);
  ~Attachment();

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;

  GLenum GetTarget() const noexcept;

 protected:
  GameContext& context_;

  int width_ = 0;
  int height_ = 0;
};
}  // namespace framebuffer
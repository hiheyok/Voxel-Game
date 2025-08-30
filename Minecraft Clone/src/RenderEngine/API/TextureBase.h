#pragma once

#include <gl/glew.h>

// Base class for all textures
class TextureBase {
 public:
  TextureBase() = default;
  virtual ~TextureBase() = default;

  TextureBase(const TextureBase&) = delete;
  TextureBase& operator=(const TextureBase&) = delete;
  TextureBase(TextureBase&&) = delete;
  TextureBase& operator=(TextureBase&&) = delete;

  virtual GLuint GetFormat() const noexcept = 0;
  virtual GLuint GetId() const noexcept = 0;
  virtual GLenum GetTarget() const noexcept = 0;
  virtual GLuint64 GetHandle() const noexcept = 0;
};

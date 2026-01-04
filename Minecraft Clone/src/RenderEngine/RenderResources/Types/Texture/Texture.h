#pragma once

#include <gl/glew.h>

#include <cstdint>

#include "RenderEngine/API/TextureBase.h"
#include "RenderEngine/RenderResources/RenderResource.h"

class GameContext;
class TextureSource;

// This is designed as an interface to manage all of the different types
// textures (e.g. TextureArray, Texture2D, TextureAtlas)
class Texture : public RenderResource, public TextureBase {
 public:
  virtual ~Texture();

  GLuint GetId() const noexcept override;
  GLenum GetTarget() const noexcept override;
  GLuint64 GetHandle() const noexcept override;
  GLuint GetFormat() const noexcept override;

  void Load() override;

 protected:
  Texture(GameContext& context, const std::string& key, GLenum target);

  virtual void LoadTexture() = 0;

  GameContext& context_;

  GLuint id_;
  GLenum target_;
  GLuint64 handle_;
};

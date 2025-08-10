#pragma once

#include <gl/glew.h>

#include "RenderEngine/RenderResources/RenderResource.h"

class GameContext;
class TextureSource;

// This is designed as an interface to manage all of the different types
// textures (e.g. TextureArray, Texture2D, TextureAtlas)
class TextureV2 : public RenderResource {
 public:
  virtual ~TextureV2();

  GLuint GetId() const noexcept;
  GLenum GetTarget() const noexcept;

 protected:
  TextureV2(GameContext& context, const std::string& key, GLenum target);

  GameContext& context_;

  GLuint id_;
  GLenum target_;
};

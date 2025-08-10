#include "RenderEngine/RenderResources/Types/Texture/Texture.h"

TextureV2::TextureV2(GameContext& context, const std::string& key,
                     GLenum target)
    : RenderResource{key}, context_{context}, target_{target} {
  glGenTextures(1, &id_);
}

TextureV2::~TextureV2() {
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
  }
}

GLuint TextureV2::GetId() const noexcept { return id_; }

GLuint TextureV2::GetTarget() const noexcept { return target_; }

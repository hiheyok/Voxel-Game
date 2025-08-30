#include "RenderEngine/RenderResources/Types/Texture/Texture.h"

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

TextureV2::TextureV2(GameContext& context, const std::string& key,
                     GLenum target)
    : RenderResource{key}, TextureBase{}, context_{context}, target_{target} {
  glGenTextures(1, &id_);
  handle_ = 0;
}

TextureV2::~TextureV2() {
  if (handle_ != 0) {
    glMakeTextureHandleNonResidentARB(handle_);
  }
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
  }
}

GLuint TextureV2::GetId() const noexcept { return id_; }

GLuint TextureV2::GetTarget() const noexcept { return target_; }

GLuint64 TextureV2::GetHandle() const noexcept { return handle_; }

GLuint TextureV2::GetFormat() const noexcept { return GL_RGBA8; }

void TextureV2::Load() {
  LoadTexture();
  handle_ = glGetTextureHandleARB(id_);
  glMakeTextureHandleResidentARB(handle_);
  LOG_DEBUG("Created texture handle: {} for {}", handle_, id_);
}

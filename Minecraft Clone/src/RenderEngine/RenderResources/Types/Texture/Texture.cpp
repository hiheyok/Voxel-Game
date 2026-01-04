#include "RenderEngine/RenderResources/Types/Texture/Texture.h"

#include <string>

#include "Core/GameContext/GameContext.h"
#include "RenderEngine/API/TextureBase.h"
#include "RenderEngine/RenderResources/RenderResource.h"
#include "Utils/LogUtils.h"

using std::string;

Texture::Texture(GameContext& context, const string& key, GLenum target)
    : RenderResource{key}, TextureBase{}, context_{context}, target_{target} {
  glGenTextures(1, &id_);
  handle_ = 0;
}

Texture::~Texture() {
  if (handle_ != 0) {
    glMakeTextureHandleNonResidentARB(handle_);
  }
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
  }
}

GLuint Texture::GetId() const noexcept { return id_; }

GLuint Texture::GetTarget() const noexcept { return target_; }

GLuint64 Texture::GetHandle() const noexcept { return handle_; }

GLuint Texture::GetFormat() const noexcept { return GL_RGBA8; }

void Texture::Load() {
  LoadTexture();
  handle_ = glGetTextureHandleARB(id_);
  glMakeTextureHandleResidentARB(handle_);
  LOG_DEBUG("Created texture handle: {} for {}", handle_, id_);
}

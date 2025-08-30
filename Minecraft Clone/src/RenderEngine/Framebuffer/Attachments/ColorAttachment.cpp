#include "RenderEngine/Framebuffer/Attachments/ColorAttachment.h"

using namespace framebuffer;

ColorAttachment::ColorAttachment(GameContext& context, int width, int height)
    : Attachment{context, width, height} {
  Create();
}
ColorAttachment::~ColorAttachment() {
  if (handle_ != 0) {
    glMakeTextureHandleNonResidentARB(handle_);
  }
  if (id_ != 0) {
    glDeleteTextures(1, &id_);
  }
}

void ColorAttachment::Attach(int index) const {
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index,
                         GetTarget(), id_, 0);
}

GLuint ColorAttachment::GetId() const noexcept { return id_; }

GLuint64 ColorAttachment::GetHandle() const noexcept { return handle_; }

void ColorAttachment::Create() {
  static constexpr int kMipMapLevel = 4;

  glGenTextures(1, &id_);
  glBindTexture(GL_TEXTURE_2D, id_);
  glTexStorage2D(GL_TEXTURE_2D, kMipMapLevel, GL_RGBA8, width_, height_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  handle_ = glGetTextureHandleARB(id_);
  glMakeTextureHandleResidentARB(handle_);
}

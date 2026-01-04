#include "RenderEngine/Framebuffer/RenderTargetTexture.h"

#include <memory>

#include "RenderEngine/API/TextureBase.h"
#include "RenderEngine/Framebuffer/Attachments/ColorAttachment.h"

using framebuffer::ColorAttachment;
using std::make_unique;
using std::unique_ptr;

RenderTargetTexture::RenderTargetTexture() { is_texture_independent_ = false; }

RenderTargetTexture::~RenderTargetTexture() {}

GLuint RenderTargetTexture::GetId() const noexcept { return id_; }

GLenum RenderTargetTexture::GetTarget() const noexcept { return GL_TEXTURE_2D; }

GLuint64 RenderTargetTexture::GetHandle() const noexcept { return handle_; }

GLuint RenderTargetTexture::GetFormat() const noexcept { return GL_RGBA8; }

int RenderTargetTexture::GetWidth() const noexcept { return width_; }

int RenderTargetTexture::GetHeight() const noexcept { return height_; }

unique_ptr<RenderTargetTexture> RenderTargetTexture::CreateColorTexture(
    const ColorAttachment& attachment) {
  auto texture = make_unique<RenderTargetTexture>();
  texture->id_ = attachment.GetId();
  texture->handle_ = attachment.GetHandle();
  texture->is_texture_independent_ = false;
  texture->height_ = attachment.GetHeight();
  texture->width_ = attachment.GetWidth();
  return texture;
}

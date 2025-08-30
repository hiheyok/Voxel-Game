#include "RenderEngine/Framebuffer/RenderTarget.h"

#include <gl/glew.h>

#include <memory>

#include "RenderEngine/Framebuffer/Attachments/ColorAttachment.h"
#include "RenderEngine/Framebuffer/Attachments/DepthStencilAttachment.h"
#include "RenderEngine/Framebuffer/RenderTargetTexture.h"

using framebuffer::ColorAttachment;
using framebuffer::DepthStencilAttachment;
using std::make_unique;

RenderTarget::RenderTarget(GameContext& context, int width, int height)
    : context_{context}, width_{width}, height_{height} {
  Create();
}
RenderTarget::~RenderTarget() {}

void RenderTarget::Bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glGetIntegerv(GL_VIEWPORT, prev_viewport);
  glViewport(0, 0, width_, height_);
}

void RenderTarget::Unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2],
             prev_viewport[3]);
}

void RenderTarget::ClearBuffers() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderTarget::Resize(int width, int height) {
  width_ = width;
  height_ = height;
  clr_attachment_ = make_unique<ColorAttachment>(context_, width, height);
  ds_attachment_ = make_unique<DepthStencilAttachment>(context_, width, height);

  Bind();
  clr_attachment_->Attach(0);
  ds_attachment_->Attach();
  Unbind();

  clr_texture_ = RenderTargetTexture::CreateColorTexture(*clr_attachment_);
}

RenderHandle<RenderTargetTexture> RenderTarget::GetColorTexture() const {
  return RenderHandle<RenderTargetTexture>(clr_texture_.get());
}

void RenderTarget::Create() {
  glGenFramebuffers(1, &fbo_);
  Resize(width_, height_);
}

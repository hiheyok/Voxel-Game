#pragma once

#include <gl/glew.h>

#include <memory>
#include <vector>

#include "RenderEngine/RenderResources/RenderHandle.h"

class GameContext;
class TextureBase;
class RenderTargetTexture;

namespace framebuffer {
class Attachment;
class ColorAttachment;
class DepthStencilAttachment;
};  // namespace framebuffer

class RenderTarget {
 public:
  RenderTarget(GameContext&, int width, int height);
  ~RenderTarget();

  RenderTarget(const RenderTarget&) = delete;
  RenderTarget& operator=(const RenderTarget&) = delete;
  RenderTarget(RenderTarget&&) = delete;
  RenderTarget& operator=(RenderTarget&&) = delete;

  void Bind();
  void Unbind();
  void ClearBuffers();

  void Resize(int width, int height);

  RenderHandle<RenderTargetTexture> GetColorTexture() const noexcept;

 private:
  void Create();

  GameContext& context_;

  GLuint fbo_;

  int width_, height_;

  // Used to contain the previous viewport sizes when unbinding the framebuffer
  GLint prev_viewport[4]{};

  std::unique_ptr<RenderTargetTexture> clr_texture_;
  std::unique_ptr<framebuffer::ColorAttachment> clr_attachment_;
  std::unique_ptr<framebuffer::DepthStencilAttachment> ds_attachment_;
};

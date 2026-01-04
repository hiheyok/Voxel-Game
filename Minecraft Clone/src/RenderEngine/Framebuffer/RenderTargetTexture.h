#include <gl/glew.h>

#include <memory>

#include "RenderEngine/API/TextureBase.h"

namespace framebuffer {
class Attachment;
class ColorAttachment;
class DepthStencilAttachment;
};  // namespace framebuffer

class RenderTargetTexture : public TextureBase {
 public:
  RenderTargetTexture();
  ~RenderTargetTexture();

  GLuint GetId() const noexcept override;
  GLenum GetTarget() const noexcept override;
  GLuint64 GetHandle() const noexcept override;
  GLuint GetFormat() const noexcept override;

  int GetHeight() const noexcept;
  int GetWidth() const noexcept;

 private:
  friend class RenderTarget;

  static std::unique_ptr<RenderTargetTexture> CreateColorTexture(
      const framebuffer::ColorAttachment&);

  bool is_texture_independent_;

  int height_;
  int width_;

  GLuint id_;
  GLuint64 handle_;
};
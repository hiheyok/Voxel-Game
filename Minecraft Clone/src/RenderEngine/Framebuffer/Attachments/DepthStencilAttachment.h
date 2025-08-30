#include "RenderEngine/Framebuffer/Attachments/Attachment.h"

class GameContext;

namespace framebuffer {

class DepthStencilAttachment : public Attachment {
 public:
  DepthStencilAttachment(GameContext&, int width, int height);
  ~DepthStencilAttachment();

  void Attach() const;
  GLuint GetId() const noexcept;

 private:
  void Create();
  GLuint id_;
};

};  // namespace framebuffer

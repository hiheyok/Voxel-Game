#include "RenderEngine/Framebuffer/Attachments/Attachment.h"
namespace framebuffer {

class ColorAttachment : public Attachment {
 public:
  ColorAttachment(GameContext&, int width, int height);
  ~ColorAttachment();

  void Attach(int index) const;
  GLuint GetId() const noexcept;
  GLuint64 GetHandle() const noexcept;

 private:
  void Create();

  GLuint64 handle_;
  GLuint id_;
};
}  // namespace framebuffer
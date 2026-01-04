#include "RenderEngine/Framebuffer/Attachments/DepthStencilAttachment.h"

#include "RenderEngine/API/TextureBase.h"
#include "RenderEngine/Framebuffer/Attachments/Attachment.h"

using namespace framebuffer;

DepthStencilAttachment::DepthStencilAttachment(GameContext& context, int width,
                                               int height)
    : Attachment{context, width, height} {
  Create();
}

DepthStencilAttachment::~DepthStencilAttachment() {
  if (id_ != 0) {
    glDeleteRenderbuffers(1, &id_);
  }
}

void DepthStencilAttachment::Attach() const {
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, id_);
}
GLuint DepthStencilAttachment::GetId() const noexcept { return id_; }

void DepthStencilAttachment::Create() {
  glGenRenderbuffers(1, &id_);
  glBindRenderbuffer(GL_RENDERBUFFER, id_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
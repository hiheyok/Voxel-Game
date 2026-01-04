#include "RenderEngine/Framebuffer/Attachments/Attachment.h"

#include "RenderEngine/API/TextureBase.h"

using namespace framebuffer;

Attachment::Attachment(GameContext& context, int width, int height)
    : context_{context}, width_{width}, height_{height} {}

Attachment::~Attachment() = default;

int Attachment::GetWidth() const noexcept { return width_; }

int Attachment::GetHeight() const noexcept { return height_; }

GLenum Attachment::GetTarget() const noexcept { return GL_TEXTURE_2D; }
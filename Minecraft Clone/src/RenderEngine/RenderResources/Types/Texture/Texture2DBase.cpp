#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

Texture2DBase::Texture2DBase(GameContext& context, const std::string& key,
                             GLenum target)
    : TextureV2{context, key, target}, width_{0}, height_{0} {}

Texture2DBase::~Texture2DBase() = default;

int Texture2DBase::GetWidth() const noexcept { return width_; }

int Texture2DBase::GetHeight() const noexcept { return height_; }

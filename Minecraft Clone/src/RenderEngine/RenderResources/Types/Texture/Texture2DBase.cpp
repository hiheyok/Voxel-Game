#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

Texture2DBase::Texture2DBase(GameContext& context, const std::string& key)
    : TextureV2{context, key, GL_TEXTURE_2D}, width_{0}, height_{0} {}

Texture2DBase::Texture2DBase(GameContext& context, const std::string& key,
                             int width, int height)
    : TextureV2{context, key, GL_TEXTURE_2D}, width_{width}, height_{height} {}

Texture2DBase::~Texture2DBase() = default;

int Texture2DBase::GetWidth() const noexcept { return width_; }

int Texture2DBase::GetHeight() const noexcept { return height_; }

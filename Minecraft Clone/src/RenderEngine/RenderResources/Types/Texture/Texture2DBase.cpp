#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

#include <string>

#include "RenderEngine/RenderResources/Types/Texture/Texture.h"

using std::string;

Texture2DBase::Texture2DBase(GameContext& context, const string& key)
    : Texture{context, key, GL_TEXTURE_2D}, width_{0}, height_{0} {}

Texture2DBase::Texture2DBase(GameContext& context, const string& key, int width,
                             int height)
    : Texture{context, key, GL_TEXTURE_2D}, width_{width}, height_{height} {}

Texture2DBase::~Texture2DBase() = default;

int Texture2DBase::GetWidth() const noexcept { return width_; }

int Texture2DBase::GetHeight() const noexcept { return height_; }

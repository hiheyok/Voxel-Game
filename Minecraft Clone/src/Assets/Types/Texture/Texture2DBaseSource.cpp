#include "Assets/Types/Texture/Texture2DBaseSource.h"

Texture2DBaseSource::Texture2DBaseSource(GameContext& context,
                                         const std::string& asset_key)
    : TextureSource{context, asset_key} {}

Texture2DBaseSource::~Texture2DBaseSource() = default;

int Texture2DBaseSource::GetWidth() const noexcept { return width_; }

int Texture2DBaseSource::GetHeight() const noexcept { return height_; }
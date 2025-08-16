#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"

#include <cassert>

#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureSprite.h"
#include "Utils/LogUtils.h"

TextureAtlasV2::TextureAtlasV2(GameContext& context, const std::string& key,
                               AssetHandle<TextureAtlasSource> source)
    : Texture2DBase{context, key}, source_{source} {}

TextureAtlasV2::~TextureAtlasV2() = default;

TextureSprite TextureAtlasV2::GetSprite(const std::string& sprite_name) const {
  const auto& it = sprites_.find(sprite_name);
  assert(it != sprites_.end());
  TextureSprite sprite = it->second;
  sprite.handle_ = handle_;
  return sprite;
}

void TextureAtlasV2::LoadTexture() {
  // Load it to the gpu
  glBindTexture(target_, id_);
  glTexImage2D(target_, 0, source_->GetFormat(), source_->GetWidth(),
               source_->GetHeight(), 0, source_->GetFormat(), GL_UNSIGNED_BYTE,
               source_->GetData());

  glGenerateMipmap(target_);
  glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  LOG_DEBUG("Loaded Texture Atlas: {}", id_);
  width_ = source_->GetWidth();
  height_ = source_->GetHeight();

  // Then process and hold all of the sprite data
  auto sprites = source_->GetAllSprites();
  for (const auto& sprite : sprites) {
    TextureSprite s;
    s.texture_id_ = id_;
    s.uv_beg_ = sprite.uv_beg_;
    s.uv_end_ = sprite.uv_end_;
    sprites_[sprite.name_] = s;
  }
}
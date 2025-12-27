#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"

#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureSprite.h"
#include "Utils/Assert.h"
#include "Utils/LogUtils.h"

TextureAtlas::TextureAtlas(GameContext& context, const std::string& key,
                           AssetHandle<TextureAtlasSource> source)
    : Texture2DBase{context, key}, source_{source} {}

TextureAtlas::~TextureAtlas() = default;

TextureSprite TextureAtlas::GetSprite(const std::string& sprite_name) const {
  const auto& it = sprites_.find(sprite_name);
  GAME_ASSERT(it != sprites_.end(), "Sprite not found in atlas");
  TextureSprite sprite = it->second;
  sprite.handle_ = handle_;
  return sprite;
}

void TextureAtlas::LoadTexture() {
  // Load it to the gpu
  glBindTexture(target_, id_);
  glTexImage2D(target_, 0, source_->GetFormat(), source_->GetWidth(),
               source_->GetHeight(), 0, source_->GetFormat(), GL_UNSIGNED_BYTE,
               source_->GetData());

  glGenerateMipmap(target_);
  glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
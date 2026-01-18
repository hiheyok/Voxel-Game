#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"

#include <string>

#include "Assets/AssetHandle.h"
#include "Assets/Types/Texture/TextureAtlasSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureSprite.h"
#include "Utils/Assert.h"
#include "Utils/LogUtils.h"

using std::string;

TextureAtlas::TextureAtlas(GameContext& context, const string& key,
                           AssetHandle<TextureAtlasSource> source)
    : Texture2DBase{context, key}, source_{source} {}

TextureAtlas::~TextureAtlas() = default;

TextureSprite TextureAtlas::GetSprite(const string& sprite_name) const {
  const auto& it = sprite_indices_.find(sprite_name);
  GAME_ASSERT(it != sprite_indices_.end(), "Sprite not found in atlas");
  TextureSprite sprite = sprites_[it->second];
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
  sprites_.reserve(sprites.size());
  for (const auto& sprite : sprites) {
    TextureSprite s;
    s.texture_id_ = id_;
    s.uv_beg_ = sprite.uv_beg_;
    s.uv_end_ = sprite.uv_end_;
    sprite_indices_[sprite.name_] = sprites_.size();
    sprites_.push_back(s);
  }
}
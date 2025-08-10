#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"

#include "Assets/Types/Texture/Texture2DSource.h"
#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

Texture2DV2::Texture2DV2(GameContext& context, const std::string& key,
                         AssetHandle<Texture2DSource> handle)
    : Texture2DBase{context, key, GL_TEXTURE_2D}, handle_{handle} {}

Texture2DV2::~Texture2DV2() = default;

void Texture2DV2::Load() {
  glBindTexture(target_, id_);
  glTexImage2D(target_, 0, handle_->GetFormat(), handle_->GetWidth(),
               handle_->GetHeight(), 0, handle_->GetFormat(), GL_UNSIGNED_BYTE,
               handle_->GetData());

  glGenerateMipmap(target_);
  glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  LOG_DEBUG("Loaded 2D Texture: {}", id_);
  width_ = handle_->GetWidth();
  height_ = handle_->GetHeight();
}
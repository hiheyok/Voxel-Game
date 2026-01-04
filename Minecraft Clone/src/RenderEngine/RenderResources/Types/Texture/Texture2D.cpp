#include "RenderEngine/RenderResources/Types/Texture/Texture2D.h"

#include <string>

#include "Assets/AssetHandle.h"
#include "Assets/Types/Texture/Texture2DSource.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"
#include "Utils/LogUtils.h"

Texture2D::Texture2D(GameContext& context, const std::string& key,
                     AssetHandle<Texture2DSource> source)
    : Texture2DBase{context, key}, source_{source} {}

Texture2D::~Texture2D() = default;

void Texture2D::LoadTexture() {
  glBindTexture(target_, id_);
  glTexImage2D(target_, 0, source_->GetFormat(), source_->GetWidth(),
               source_->GetHeight(), 0, source_->GetFormat(), GL_UNSIGNED_BYTE,
               source_->GetData());

  glGenerateMipmap(target_);
  glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  LOG_DEBUG("Loaded 2D Texture: {}", id_);
  width_ = source_->GetWidth();
  height_ = source_->GetHeight();
}
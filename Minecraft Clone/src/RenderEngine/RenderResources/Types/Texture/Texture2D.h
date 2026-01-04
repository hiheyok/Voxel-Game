#pragma once

#include "Assets/AssetHandle.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

class Texture2DSource;

class Texture2D : public Texture2DBase {
 public:
  Texture2D(GameContext& context, const std::string& key,
            AssetHandle<Texture2DSource> source);
  ~Texture2D();

 private:
  void LoadTexture() override;

  AssetHandle<Texture2DSource> source_;
};

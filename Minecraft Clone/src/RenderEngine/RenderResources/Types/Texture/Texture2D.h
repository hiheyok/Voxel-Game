#pragma once

#include "Assets/AssetHandle.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

class Texture2DSource;

class Texture2DV2 : public Texture2DBase {
 public:
  Texture2DV2(GameContext& context, const std::string& key,
              AssetHandle<Texture2DSource> source);
  ~Texture2DV2();

 private:
  void LoadTexture() override;

  AssetHandle<Texture2DSource> source_;
};

#pragma once
#include <glm/vec2.hpp>

#include "Assets/AssetHandle.h"
#include "Core/Typenames.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

class GameContext;
class TextureAtlasSource;

struct TextureSprite;

class TextureAtlasV2 : public Texture2DBase {
 public:
  TextureAtlasV2(GameContext&, const std::string&,
                 AssetHandle<TextureAtlasSource>);
  ~TextureAtlasV2();

  TextureSprite GetSprite(const std::string& sprite_name) const;

 private:
  void LoadTexture() override;

  FastHashMap<std::string, TextureSprite> sprites_;

  AssetHandle<TextureAtlasSource> source_;
};

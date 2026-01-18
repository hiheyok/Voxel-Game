#pragma once
#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "Assets/AssetHandle.h"
#include "Core/Typenames.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

class GameContext;
class TextureAtlasSource;

struct TextureSprite;

class TextureAtlas : public Texture2DBase {
 public:
  TextureAtlas(GameContext&, const std::string&,
               AssetHandle<TextureAtlasSource>);
  ~TextureAtlas();

  TextureSprite GetSprite(const std::string& sprite_name) const;

 private:
  void LoadTexture() override;

  // Store sprites in vector (respects alignas(32)) and use hash map for
  // name-to-index lookup (robin_hood can't store alignas(32) types directly)
  std::vector<TextureSprite> sprites_;
  FastHashMap<std::string, size_t> sprite_indices_;

  AssetHandle<TextureAtlasSource> source_;
};

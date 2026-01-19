#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "Core/Typenames.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"

class GameContext;
class TextureSprite;
class ResourceLocation;
class Stitcher;
class RenderTargetTexture;

class DynamicAtlas : public Texture2DBase {
 public:
  DynamicAtlas(GameContext&, const std::string& key, int width, int height);
  ~DynamicAtlas();

  std::optional<TextureSprite> GetSprite(
      const ResourceLocation& location) const;
  std::optional<TextureSprite> AddSprite(
      const ResourceLocation& location,
      RenderHandle<RenderTargetTexture> texture);
  void RemoveSprite(const ResourceLocation& location);

 private:
  void LoadTexture() override;

  std::unique_ptr<Stitcher> stitcher_;
  
  // Store sprites in vector (respects alignas(32)) and use hash map for
  // location-to-index lookup (robin_hood can't store alignas(32) types)
  std::vector<TextureSprite> sprites_;
  FastHashMap<ResourceLocation, size_t> sprite_indices_;
  std::vector<ResourceLocation> index_to_location_;  // Reverse mapping for O(1) removal
};

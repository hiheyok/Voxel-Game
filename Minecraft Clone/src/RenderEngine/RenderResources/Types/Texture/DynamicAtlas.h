#pragma once

#include <memory>
#include <optional>

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
  FastHashMap<ResourceLocation, TextureSprite> sprites_;
};

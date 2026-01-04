#pragma once

#include <memory>

#include "RenderEngine/RenderResources/RenderHandle.h"

class GameContext;
class Item;
class DynamicAtlas;
class TextureSprite;
class ItemRender;
class RenderTarget;
class ResourceLocation;
class TextureV2;

class ItemIconManager {
 public:
  ItemIconManager(GameContext&);
  ~ItemIconManager();

  ItemIconManager(const ItemIconManager&) = delete;
  ItemIconManager& operator=(const ItemIconManager&) = delete;
  ItemIconManager(ItemIconManager&&) = delete;
  ItemIconManager& operator=(ItemIconManager&&) = delete;

  // Get or render item icon and returns the sprite
  TextureSprite GetAddItemIcon(const Item& item);

  // Remove icon if not used
  void RemoveItemIcon(const Item& item);

  RenderHandle<TextureV2> GetAtlasTexture() const noexcept;

 private:
  TextureSprite AddItemIcon(const ResourceLocation& location, const Item& item);

  // Enough for 256 icons
  static constexpr int kAtlasSize = 2048;
  static constexpr int kIconSize = 128;

  GameContext& context_;
  std::unique_ptr<DynamicAtlas> atlas_;
  std::unique_ptr<ItemRender> item_render_;
  std::unique_ptr<RenderTarget> render_target_;
};

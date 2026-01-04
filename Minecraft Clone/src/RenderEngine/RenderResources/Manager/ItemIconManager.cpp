
#include "RenderEngine/RenderResources/Manager/ItemIconManager.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"
#include "Level/Item/Item.h"
#include "RenderEngine/Framebuffer/RenderTarget.h"
#include "RenderEngine/ItemRender/ItemRender.h"
#include "RenderEngine/RenderResources/Types/Texture/DynamicAtlas.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureSprite.h"

using std::make_unique;
using std::optional;
using std::vector;

ItemIconManager::ItemIconManager(GameContext& context)
    : context_{context},
      atlas_{make_unique<DynamicAtlas>(context, "item_icons", kAtlasSize,
                                       kAtlasSize)},
      item_render_{make_unique<ItemRender>(context)},
      render_target_{make_unique<RenderTarget>(context, kIconSize, kIconSize)} {
  atlas_->Load();
}

ItemIconManager::~ItemIconManager() = default;

TextureSprite ItemIconManager::GetAddItemIcon(const Item& item) {
  const vector<Block*>& blocks = context_.blocks_->GetBlockTypeList();
  ResourceLocation location{blocks[item.GetBlock()]->block_name_};

  optional<TextureSprite> sprite = atlas_->GetSprite(location);

  if (!sprite.has_value()) {
    sprite = AddItemIcon(location, item);
  }

  return sprite.value();
}

TextureSprite ItemIconManager::AddItemIcon(const ResourceLocation& location,
                                           const Item& item) {
  render_target_->Bind();
  render_target_->ClearBuffers();
  item_render_->RenderModel(item);
  render_target_->Unbind();

  RenderHandle<RenderTargetTexture> out = render_target_->GetColorTexture();
  optional<TextureSprite> sprite = atlas_->AddSprite(location, out);

  if (sprite.has_value()) {
    return sprite.value();
  } else {
    TextureSprite null_sprite;
    null_sprite.handle_ = atlas_->GetHandle();
    null_sprite.texture_id_ = atlas_->GetId();
    null_sprite.uv_beg_ = {0.0f, 0.0f};
    null_sprite.uv_end_ = {1.0f, 1.0f};
    return null_sprite;
  }
}

void ItemIconManager::RemoveItemIcon(const Item& item) {
  const vector<Block*>& blocks = context_.blocks_->GetBlockTypeList();
  ResourceLocation location{blocks[item.GetBlock()]->block_name_};
  atlas_->RemoveSprite(location);
}

RenderHandle<TextureV2> ItemIconManager::GetAtlasTexture()
    const noexcept {
  return RenderHandle<TextureV2>{atlas_.get()};
}

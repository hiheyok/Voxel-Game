#include "RenderEngine/RenderResources/Types/Texture/DynamicAtlas.h"

#include <optional>
#include <string>

#include "Assets/Types/Texture/Atlas/Sticher.h"
#include "Core/GameContext/GameContext.h"
#include "RenderEngine/Framebuffer/RenderTarget.h"
#include "RenderEngine/Framebuffer/RenderTargetTexture.h"
#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture.h"
#include "RenderEngine/RenderResources/Types/Texture/Texture2DBase.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureSprite.h"
#include "Utils/LogUtils.h"

using std::make_unique;
using std::nullopt;
using std::optional;
using std::string;

DynamicAtlas::DynamicAtlas(GameContext& context, const string& key, int width,
                           int height)
    : Texture2DBase{context, key, width, height},
      stitcher_{make_unique<Stitcher>(context, width, height, 16, false)} {}

DynamicAtlas::~DynamicAtlas() = default;

optional<TextureSprite> DynamicAtlas::GetSprite(
    const ResourceLocation& location) const {
  const auto& it = sprite_indices_.find(location);
  if (it == sprite_indices_.end()) {
    return nullopt;
  } else {
    return sprites_[it->second];
  }
}

optional<TextureSprite> DynamicAtlas::AddSprite(
    const ResourceLocation& location,
    RenderHandle<RenderTargetTexture> texture) {
  // Verify that the texture format matches

  GLenum format = texture->GetFormat();

  if (format != GL_RGBA && format != GL_RGBA8 && format != GL_RGBA12 &&
      format != GL_RGBA16) {
    LOG_DEBUG("Cannot add sprite. Incompatable texture format.");
    return nullopt;
  }

  int sprite_width = texture->GetWidth();
  int sprite_height = texture->GetHeight();

  // Find placement
  auto [x, y] = stitcher_->PlaceItem(location, sprite_width, sprite_height);

  if (x == -1 || y == -1) {
    LOG_DEBUG("Cannot add sprite. Texture atlas is full.");
    return nullopt;
  }

  glCopyImageSubData(texture->GetId(), texture->GetTarget(), 0, 0, 0, 0, id_,
                     target_, 0, x, y, 0, sprite_width, sprite_height, 1);

  TextureSprite sprite;
  sprite.handle_ = handle_;
  sprite.texture_id_ = id_;
  sprite.uv_beg_ = {static_cast<float>(x) / width_,
                    static_cast<float>(y) / height_};
  sprite.uv_end_ = {static_cast<float>(x + sprite_width) / width_,
                    static_cast<float>(y + sprite_height) / height_};
  sprite_indices_[location] = sprites_.size();
  sprites_.push_back(sprite);
  index_to_location_.push_back(location);

  return sprite;
}

void DynamicAtlas::RemoveSprite(const ResourceLocation& location) {
  const auto& it = sprite_indices_.find(location);

  if (it == sprite_indices_.end()) {
    LOG_WARN("Unable to remove sprite: {}. Doesn't exist.",
             location.ToString());
    return;
  }

  stitcher_->RemoveItem(location);
  
  // Swap-and-pop to remove from vector - O(1) using reverse mapping
  size_t idx = it->second;
  size_t last_idx = sprites_.size() - 1;
  if (idx != last_idx) {
    sprites_[idx] = std::move(sprites_.back());
    index_to_location_[idx] = index_to_location_.back();
    // Update the hash map for the moved element
    sprite_indices_[index_to_location_[idx]] = idx;
  }
  sprites_.pop_back();
  index_to_location_.pop_back();
  sprite_indices_.erase(it);
}

void DynamicAtlas::LoadTexture() {
  glBindTexture(target_, id_);
  glTexStorage2D(target_, 1, GL_RGBA8, width_, height_);

  glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(target_, 0);
}

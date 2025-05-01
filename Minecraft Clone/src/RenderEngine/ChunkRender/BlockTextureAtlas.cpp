// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"

BlockTextureAtlas::BlockTextureAtlas(int imagesX, int imagesY,
                                     int individual_image_x_,
                                     int individual_image_y_)
    : TextureAtlas{imagesX * individual_image_x_, imagesY * individual_image_y_,
                   individual_image_x_, individual_image_x_} {}

BlockTextureAtlas::~BlockTextureAtlas() = default;

int BlockTextureAtlas::AddBlockTexture(const ResourceLocation& location) {
  const auto& it = texture_metadata_.find(location);
  if (it != texture_metadata_.end()) {
    return it->second.texture_id_;
  } else {
    size_t textureCount = GetTextureCount();
    std::optional<RawTextureData> texture =
        AddTextureToAtlas(location.GetPath());
    if (!texture.has_value()) return -1;

    TextureMetadata metadata;
    metadata.texture_id_ = textureCount + 1;
    metadata.texture_count_ = GetTextureCount() - textureCount;
    metadata.fully_transparent_ = texture->HasFullyTransparentPixels();
    metadata.partially_transparent_ = texture->HasPartiallyTransparentPixels();

    texture_metadata_[location] = metadata;
    return textureCount + 1;
  }
}

int BlockTextureAtlas::GetTextureID(const ResourceLocation& location) const {
  const auto& it = texture_metadata_.find(location);
  if (it == texture_metadata_.end()) {
    g_logger.LogError("BlockTextureAtlas::GetTextureID",
                      "Tried to search for invalid texture!");
  }
  return it->second.texture_id_;
}

int BlockTextureAtlas::GetBlockTextureCount(
    const ResourceLocation& location) const {
  const auto& it = texture_metadata_.find(location);
  if (it == texture_metadata_.end()) {
    g_logger.LogError("BlockTextureAtlas::GetTextureID",
                      "Tried to search for invalid texture!");
  }
  return it->second.texture_count_;
}

bool BlockTextureAtlas::IsTexturePartiallyTransparent(
    const ResourceLocation& location) const {
  const auto& it = texture_metadata_.find(location);
  if (it == texture_metadata_.end()) {
    g_logger.LogError("BlockTextureAtlas::IsTexturePartiallyTransparent",
                      "Tried to search for invalid texture!");
  }
  return it->second.partially_transparent_;
}

bool BlockTextureAtlas::IsTextureFullyTransparent(
    const ResourceLocation& location) const {
  const auto& it = texture_metadata_.find(location);
  if (it == texture_metadata_.end()) {
    g_logger.LogError("BlockTextureAtlas::IsTextureFullyTransparent",
                      "Tried to search for invalid texture!");
  }
  return it->second.fully_transparent_;
}

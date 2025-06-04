// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <optional>

#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"

class GameContext;

/*
 * This manages all of the block textures
 */
class BlockTextureAtlas : public TextureAtlas {
 public:
  BlockTextureAtlas(GameContext&, int imagesX, int imagesY, int individual_image_x_,
                    int individual_image_y_);
  ~BlockTextureAtlas();

  // This takes in the RawTextureData and returns the textureID
  int AddBlockTexture(const ResourceLocation&);
  int GetTextureID(const ResourceLocation&) const;
  int GetBlockTextureCount(const ResourceLocation&) const;
  bool IsTexturePartiallyTransparent(const ResourceLocation&) const;
  bool IsTextureFullyTransparent(const ResourceLocation&) const;

 private:
  struct TextureMetadata {
    int texture_id_ = 0;
    int texture_count_ = 0;
    bool partially_transparent_ = false;
    bool fully_transparent_ = false;
  };

  FastHashMap<ResourceLocation, TextureMetadata> texture_metadata_;
};

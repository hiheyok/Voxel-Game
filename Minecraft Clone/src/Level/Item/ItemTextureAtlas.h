// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <glm/vec2.hpp>
#include <memory>

#include "Level/Item/Item.h"
#include "RenderEngine/ItemRender/BlockItemRender.h"
#include "RenderEngine/OpenGL//Texture/Types/Texture2D.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "RenderEngine/RenderResources/RenderHandle.h"

class GameContext;
class Shader;

struct ItemUVMapping {
  glm::vec2 uv_1_;
  glm::vec2 uv_2_;
};

class ItemTextureAtlas {
 public:
  FastHashMap<int, ItemUVMapping> items_uv_map_;
  FastHashMap<ItemID, size_t> offsets_;

  explicit ItemTextureAtlas(GameContext&);
  ~ItemTextureAtlas();

  void Initialize(int atlasItemSize, int individualItemSize);
  GLuint Get() const;
  size_t GetHeight() const;
  size_t GetWidth() const;

  void AddItem(Item item);

 private:
  void RenderBlockItem(Item item);
  void StitchTexture(size_t index, ItemID ItemID);

  GameContext& context_;

  std::unique_ptr<VertexArray> vao_;
  std::unique_ptr<Buffer> ebo_;
  std::unique_ptr<Buffer> vbo_;

  BlockItemRender block_item_renderer_;
  RenderHandle<Shader> stitching_shader_;
  int individual_size_ = 0;
  int atlas_size_ = 0;
  TexturedFrameBuffer atlas_framebuffer_;
  TexturedFrameBuffer framebuffer_single_block_render_;
};

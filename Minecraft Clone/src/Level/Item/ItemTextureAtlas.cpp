// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Item/ItemTextureAtlas.h"

void ItemTextureAtlas::RenderBlockItem(Item item) {
  framebuffer_single_block_render_.BindFBO();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  block_item_renderer_.RenderBlock(item);
  framebuffer_single_block_render_.UnbindFBO();
}

void ItemTextureAtlas::StitchTexture(size_t index, ItemID ItemID) {
  int ratio = atlas_size_ / individual_size_;
  float xCoord = static_cast<float>((index % ratio));
  float yCoord = floor(static_cast<float>(index) / ratio);

  // Normalize

  xCoord = xCoord / ratio;
  yCoord = yCoord / ratio;

  float Size = 2.f / ratio;

  // Map to NDC

  xCoord = -(xCoord * 2.f - 1.f);
  yCoord = -(yCoord * 2.f - 1.f);

  // Insert Data
  float vertices[] = {
      xCoord - 0.f,  yCoord - 0.f,  0.f, 1.f,
      xCoord - Size, yCoord - 0.f,  1.f, 1.f,
      xCoord - Size, yCoord - Size, 1.f, 0.f,
      xCoord - 0.f,  yCoord - Size, 0.f, 0.f,
  };

  uint32_t indices[] = {0, 1, 2, 2, 3, 0};

  ItemUVMapping uvMap;
  uvMap.uv_1_ = glm::vec2((xCoord + 1.f) * 0.5f, (yCoord + 1.f) * 0.5f);
  uvMap.uv_2_ =
      glm::vec2((xCoord - Size + 1.f) * 0.5f, (yCoord - Size + 1.f) * 0.5f);

  uvMap.uv_1_.y = 1.f - uvMap.uv_1_.y;
  uvMap.uv_2_.y = 1.f - uvMap.uv_2_.y;

  items_uv_map_[ItemID] = uvMap;

  vbo_->InsertData(sizeof(vertices), vertices, GL_STATIC_DRAW);
  ebo_->InsertData(sizeof(indices), indices, GL_STATIC_DRAW);
  // Render
  atlas_framebuffer_.BindFBO();
  stitching_shader_.BindTexture2D(0, framebuffer_single_block_render_.texture_,
                                  "ItemTexture");

  glEnable(GL_BLEND);

  vao_->Bind();
  ebo_->Bind();
  glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t),
                 GL_UNSIGNED_INT, 0);
  vao_->Unbind();
  ebo_->Unbind();

  glDisable(GL_BLEND);
  atlas_framebuffer_.UnbindFBO();
}
ItemTextureAtlas::ItemTextureAtlas(GameContext& game_context)
    : game_context_{game_context},
      stitching_shader_{game_context},
      atlas_framebuffer_{game_context},
      framebuffer_single_block_render_{game_context},
      block_item_renderer_{game_context} {}
ItemTextureAtlas::~ItemTextureAtlas() = default;

void ItemTextureAtlas::Initialize(int atlasItemSize, int individualItemSize) {
  stitching_shader_ =
      Shader(game_context_, "assets/shaders/ItemRender/AtlasStitchVert.glsl",
             "assets/shaders/ItemRender/AtlasStitchFrag.glsl");

  individual_size_ = individualItemSize;
  atlas_size_ = atlasItemSize;

  atlas_framebuffer_.GenBuffer(atlas_size_, atlas_size_, 1, GL_RGBA);
  framebuffer_single_block_render_.GenBuffer(individual_size_, individual_size_,
                                             2, GL_RGBA);

  vbo_ = std::make_unique<Buffer>(game_context_);
  ebo_ = std::make_unique<Buffer>(game_context_);
  vao_ = std::make_unique<VertexArray>(game_context_);

  vbo_->SetType(GL_ARRAY_BUFFER);
  ebo_->SetType(GL_ELEMENT_ARRAY_BUFFER);

  vbo_->SetUsage(GL_STATIC_DRAW);
  ebo_->SetUsage(GL_STATIC_DRAW);

  vao_->EnableAttriPtr(vbo_.get(), 0, 2, GL_FLOAT, GL_FALSE, 4, 0)
      .EnableAttriPtr(vbo_.get(), 1, 2, GL_FLOAT, GL_FALSE, 4, 2);

  block_item_renderer_.Initialize();
}

void ItemTextureAtlas::AddItem(Item item) {
  offsets_[item.properties_.id_] = offsets_.size();
  RenderBlockItem(item);
  StitchTexture(offsets_.size() - 1, item.properties_.id_);
}

GLuint ItemTextureAtlas::Get() const { return atlas_framebuffer_.texture_; }

size_t ItemTextureAtlas::GetHeight() const { return atlas_size_; }

size_t ItemTextureAtlas::GetWidth() const { return atlas_size_; }

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Item/ItemTextureAtlas.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/Typenames.h"
#include "RenderEngine/ItemRender/ItemRender.h"
#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "RenderEngine/OpenGL/Render/RenderDrawElements.h"
#include "RenderEngine/OpenGL/Shader/Shader.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"

void ItemTextureAtlas::RenderBlockItem(Item item) {
  framebuffer_single_block_render_.BindFBO();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  item_render_->RenderModel(item);
  framebuffer_single_block_render_.UnbindFBO();
}

void ItemTextureAtlas::StitchTexture(size_t index, ItemID ItemID) {
  int ratio = atlas_size_ / individual_size_;
  float xCoord = static_cast<float>((index % ratio));
  float yCoord = static_cast<float>(index / ratio);

  // Normalize
  xCoord = xCoord / ratio;
  yCoord = yCoord / ratio;

  float Size = 2.f / ratio;

  // Map to NDC
  xCoord = -(xCoord * 2.f - 1.f);
  yCoord = -(yCoord * 2.f - 1.f);

  // Insert Data
  std::vector<float> vertices = {
      xCoord - 0.0f, yCoord - 0.0f, 0.0f, 1.0f,
      xCoord - Size, yCoord - 0.0f, 1.0f, 1.0f,
      xCoord - Size, yCoord - Size, 1.0f, 0.0f,
      xCoord - 0.0f, yCoord - Size, 0.0f, 0.0f,
  };

  std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};

  render_->SetData(vertices, indices);
  render_->SetIndicesCount(indices.size());
  render_->GetShader()->BindTexture2D(
      0, framebuffer_single_block_render_.texture_, "ItemTexture");

  ItemUVMapping uvMap;
  uvMap.uv_1_ = {(xCoord + 1.f) * 0.5f, (yCoord + 1.f) * 0.5f};
  uvMap.uv_2_ = {(xCoord - Size + 1.f) * 0.5f, (yCoord - Size + 1.f) * 0.5f};

  uvMap.uv_1_.y = 1.f - uvMap.uv_1_.y;
  uvMap.uv_2_.y = 1.f - uvMap.uv_2_.y;

  items_uv_map_[ItemID] = uvMap;

  // Render
  atlas_framebuffer_.BindFBO();
  render_->Render();

  glDisable(GL_BLEND);
  atlas_framebuffer_.UnbindFBO();
}

ItemTextureAtlas::ItemTextureAtlas(GameContext& context)
    : context_{context},
      atlas_framebuffer_{context},
      framebuffer_single_block_render_{context} {}
ItemTextureAtlas::~ItemTextureAtlas() = default;

// TODO(hiheyok): Need to move atlas stitching to CPU
void ItemTextureAtlas::Initialize(int atlasItemSize, int individualItemSize) {
  item_render_ = std::make_unique<ItemRender>(context_);

  individual_size_ = individualItemSize;
  atlas_size_ = atlasItemSize;

  atlas_framebuffer_.GenBuffer(atlas_size_, atlas_size_, 1, GL_RGBA);
  framebuffer_single_block_render_.GenBuffer(individual_size_, individual_size_,
                                             2, GL_RGBA);

  render_ = std::make_unique<RenderDrawElements>(context_);
  render_->SetDataAttribute(0, 2, GL_FLOAT, 4, 0);
  render_->SetDataAttribute(1, 2, GL_FLOAT, 4, 2);
  render_->SetShader("atlas_stitch_shader");
}

void ItemTextureAtlas::AddItem(Item item) {
  offsets_[item.properties_.id_] = offsets_.size();
  RenderBlockItem(item);
  StitchTexture(offsets_.size() - 1, item.properties_.id_);
}

GLuint ItemTextureAtlas::Get() const { return atlas_framebuffer_.texture_; }

size_t ItemTextureAtlas::GetHeight() const { return atlas_size_; }

size_t ItemTextureAtlas::GetWidth() const { return atlas_size_; }

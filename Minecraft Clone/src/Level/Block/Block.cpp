// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Block.h"

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "Utils/LogUtils.h"

Block::Block(GameContext& game_context) : game_context_{game_context} {
  properties_ = std::make_unique<BlockProperties>();
}
Block::~Block() = default;

void Block::InitializeBlockModel(ModelLoader& modelLoader) {
  auto tokens = Tokenize(block_name_, ':');
  std::unique_ptr<BlockModel> model = modelLoader.GetModel(ResourceLocation(
      "models/block/" + tokens.back() + ".json", tokens.front()));
  if (model == nullptr) {
    model = std::make_unique<BlockModel>();
  }
  model->is_initialized_ = true;
  model->FlattenVariables();
  model->BakeTextureRotation();
  block_model_data_ = std::move(model);
}

void Block::InitializeTexture(BlockTextureAtlas& textureAtlas) {
  for (auto& element : block_model_data_->elements_) {
    for (const auto& side : Directions<BlockPos>()) {
      const std::string& path = element.faces_[side].reference_texture_;
      if (path.length() == 0) continue;

      auto tokens = Tokenize(path, ':');

      // Load texture
      ResourceLocation location{"/textures/" + tokens.front() + ".png",
                                tokens.back()};

      int textureId = textureAtlas.AddBlockTexture(location);

      if (textureId == -1) {
        game_context_.logger_->LogWarn("BlockList::InitializeBlockModels",
                                       "Unable to load texture.");
        continue;
      }

      element.faces_[side].texture_id_ = textureId;
      element.faces_[side].texture_count_ =
          textureAtlas.GetBlockTextureCount(location);
      element.faces_[side].partially_transparent_pixel_ =
          textureAtlas.IsTexturePartiallyTransparent(location);
      element.faces_[side].fully_transparent_pixel_ =
          textureAtlas.IsTextureFullyTransparent(location);
    }
  }
}

BlockProperties::BlockProperties() = default;

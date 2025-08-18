// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Block.h"

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "RenderEngine/Models/Block/BlockModelLoader.h"
#include "RenderEngine/Models/Block/BlockModels.h"
#include "Utils/LogUtils.h"

Block::Block(GameContext& context) : context_{context} {
  properties_ = std::make_unique<BlockProperties>();
}
Block::~Block() = default;

std::unique_ptr<BlockModel> Block::InitializeBlockModel(
    BlockModelLoader& model_loader) {
  auto tokens = Tokenize(block_name_, ':');
  std::unique_ptr<BlockModel> model = model_loader.GetModel(ResourceLocation(
      "models/block/" + tokens.back() + ".json", tokens.front()));
  if (model == nullptr) {
    model = std::make_unique<BlockModel>();
  }
  model->is_initialized_ = true;
  model->FlattenVariables();
  model->BakeTextureRotation();
  return model;
}

BlockProperties::BlockProperties() = default;

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/BlockModel/BlockModelManager.h"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Level/Block/Blocks.h"
#include "RenderEngine/BlockModel/BlockModels.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "Utils/LogUtils.h"

BlockModelManager::BlockModelManager(GameContext& game_context)
    : game_context_{game_context},
      is_loaded_{false},
      model_loader_{std::make_unique<ModelLoader>(game_context)} {}
BlockModelManager::~BlockModelManager() = default;

void BlockModelManager::LoadModels() {
  static constexpr int kIndividualTexSize = 16;

  if (model_data_.size() != 0) {
    throw std::runtime_error(
        "BlockModelManager::LoadModels - Models already loaded!");
  }
  block_texture_atlas_ = std::make_unique<BlockTextureAtlas>(
      game_context_, 512, 512, kIndividualTexSize, kIndividualTexSize);

  for (BlockID id = 0; id < game_context_.blocks_->block_type_data_.size();
       ++id) {
    Block* block = game_context_.blocks_->block_type_data_[id];
    std::unique_ptr<BlockModel> model =
        block->InitializeBlockModel(*model_loader_.get());
    model_data_.emplace_back(*model.get());
  }

  // Load textures next

  for (auto& model : model_data_) {
    for (auto& element : model.elements_) {
      for (const auto& side : Directions<BlockPos>()) {
        const std::string& path = element.faces_[side].reference_texture_;
        if (path.length() == 0) continue;

        auto tokens = Tokenize(path, ':');

        // Load texture
        ResourceLocation location{"/textures/" + tokens.front() + ".png",
                                  tokens.back()};

        int textureId = block_texture_atlas_->AddBlockTexture(location);

        if (textureId == -1) {
          game_context_.logger_->LogWarn("BlockList::InitializeBlockModels",
                                         "Unable to load texture.");
          continue;
        }

        element.faces_[side].texture_id_ = textureId;
        element.faces_[side].texture_count_ =
            block_texture_atlas_->GetBlockTextureCount(location);
        element.faces_[side].partially_transparent_pixel_ =
            block_texture_atlas_->IsTexturePartiallyTransparent(location);
        element.faces_[side].fully_transparent_pixel_ =
            block_texture_atlas_->IsTextureFullyTransparent(location);
      }
    }
  }

  block_texture_atlas_->LoadToGPU();
  is_loaded_ = true;
}

const BlockModel& BlockModelManager::GetBlockModel(BlockID id) const {
  return model_data_[id];
}

uint32_t BlockModelManager::GetTextureAtlasID() const {
  if (block_texture_atlas_ == nullptr) {
    throw std::runtime_error(
        "BlockModelManager::GetTextureAtlasID - tried to access unloaded "
        "texture");
  }
  return block_texture_atlas_->Get();
}

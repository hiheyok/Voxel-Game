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
#include "RenderEngine/OpenGL/Texture/Types/Texture2DOld.h"
#include "RenderEngine/RenderResources/RenderResourceManager.h"
#include "Utils/LogUtils.h"

BlockModelManager::BlockModelManager(GameContext& context)
    : context_{context},
      is_loaded_{false},
      model_loader_{std::make_unique<ModelLoader>(context)} {}
BlockModelManager::~BlockModelManager() = default;

void BlockModelManager::LoadModels() {
  static constexpr int kIndividualTexSize = 16;

  if (model_data_.size() != 0) {
    throw std::runtime_error(
        "BlockModelManager::LoadModels - Models already loaded!");
  }
  block_texture_atlas_ = std::make_unique<BlockTextureAtlas>(
      context_, 512, 512, kIndividualTexSize, kIndividualTexSize);

  for (BlockID id = 0; id < context_.blocks_->block_type_data_.size(); ++id) {
    Block* block = context_.blocks_->block_type_data_[id];
    std::unique_ptr<BlockModel> model =
        block->InitializeBlockModel(*model_loader_.get());
    model_data_.emplace_back(*model.get());
  }

  // Load textures next

  for (auto& model : model_data_) {
    for (auto& element : model.elements_) {
      for (auto side : Directions<BlockPos>()) {
        const std::string& path = element.faces_[side].reference_texture_;
        if (path.length() == 0) continue;

        size_t idx = path.find(':');
        std::string texture_location = path.substr(idx + 1);
        std::string texture_namespace = path.substr(0, idx);

        // Load texture
        ResourceLocation location{"/textures/" + texture_location + ".png",
                                  texture_namespace};

        int id = block_texture_atlas_->AddBlockTexture(location);

        if (id == -1) {
          LOG_WARN("Unable to load texture.");
          continue;
        }

        element.faces_[side].texture_id_ = id;
        element.faces_[side].texture_count_ =
            block_texture_atlas_->GetBlockTextureCount(location);
        element.faces_[side].partially_transparent_pixel_ =
            block_texture_atlas_->IsTexturePartiallyTransparent(location);
        element.faces_[side].fully_transparent_pixel_ =
            block_texture_atlas_->IsTextureFullyTransparent(location);

        element.faces_[side].can_cull =
            element.faces_[side].fully_transparent_pixel_ ||
            element.faces_[side].partially_transparent_pixel_ ||
            element.faces_[side].reference_texture_.empty();
      }
    }
  }

  block_texture_atlas_->LoadToGPU();
  is_loaded_ = true;

  light_map_ = context_.render_resource_manager_->GetTexture2D("light_map");
}

const BlockModel& BlockModelManager::GetBlockModel(BlockID id) const {
  return model_data_[id];
}

const std::vector<BlockModel>& BlockModelManager::GetBlockModelList()
    const noexcept {
  return model_data_;
}

uint32_t BlockModelManager::GetTextureAtlasID() const {
  if (block_texture_atlas_ == nullptr) {
    throw std::runtime_error(
        "BlockModelManager::GetTextureAtlasID - tried to access unloaded "
        "texture");
  }
  return block_texture_atlas_->Get();
}

RenderHandle<Texture2DV2> BlockModelManager::GetLightMap() const {
  return light_map_;
}

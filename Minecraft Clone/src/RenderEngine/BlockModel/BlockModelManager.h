// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <cstdint>
#include <memory>

#include "Core/Typenames.h"
#include "RenderEngine/RenderResources/RenderHandle.h"

class GameContext;
class ModelLoader;
class BlockTextureAtlas;
class Texture2DV2;

struct BlockModel;

class BlockModelManager {
 public:
  explicit BlockModelManager(GameContext&);
  ~BlockModelManager();
  BlockModelManager(BlockModelManager&&) = delete;
  BlockModelManager(const BlockModelManager&) = delete;

  void LoadModels();  // Loads all of the models in the block list

  const BlockModel& GetBlockModel(BlockID id) const;
  const std::vector<BlockModel>& GetBlockModelList() const noexcept;
  uint32_t GetTextureAtlasID() const;
  RenderHandle<Texture2DV2> GetLightMap() const;

 private:
  GameContext& context_;
  bool is_loaded_;
  std::unique_ptr<ModelLoader> model_loader_;
  std::unique_ptr<BlockTextureAtlas> block_texture_atlas_;
  RenderHandle<Texture2DV2> light_map_;
  std::vector<BlockModel> model_data_;
};

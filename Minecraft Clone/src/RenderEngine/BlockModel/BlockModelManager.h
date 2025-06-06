// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <cstdint>
#include <memory>

#include "Core/Typenames.h"

class GameContext;
class ModelLoader;
class BlockTextureAtlas;
class Texture2D;

struct BlockModel;

class BlockModelManager {
 public:
  explicit BlockModelManager(GameContext&);
  ~BlockModelManager();
  BlockModelManager(BlockModelManager&&) = delete;
  BlockModelManager(const BlockModelManager&) = delete;

  void LoadModels();  // Loads all of the models in the block list

  const BlockModel& GetBlockModel(BlockID id) const;
  uint32_t GetTextureAtlasID() const;
  uint32_t GetLightMapID() const;

 private:
  GameContext& game_context_;
  bool is_loaded_;
  std::unique_ptr<ModelLoader> model_loader_;
  std::unique_ptr<BlockTextureAtlas> block_texture_atlas_;
  std::unique_ptr<Texture2D> light_map_;
  std::vector<BlockModel> model_data_;
};

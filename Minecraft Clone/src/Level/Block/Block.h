// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Core/Typenames.h"

class Dimension;
class BlockTextureAtlas;
class ModelLoader;
class GameContext;

struct BlockModel;

struct BlockProperties {
  BlockProperties();

  BlockProperties& SetIsFluid(bool is_fluid);
  BlockProperties& SetTransparency(bool is_fluid);

  bool is_fluid_ = false;
  bool is_solid_ = false;
  bool light_pass_ = false;
  bool transparency_ = false;
  int light_emission = 0;
  int opacity_ = 15;
};

class Block {
 public:
  explicit Block(GameContext&);
  virtual ~Block();

  virtual void Tick(BlockPos pos, Dimension* currentWorld) = 0;

  // For client side
  virtual std::unique_ptr<BlockModel> InitializeBlockModel(
      ModelLoader& modelLoader);
  void InitializeTexture(BlockTextureAtlas& textureAtlas);

  GameContext& context_;
  BlockID id_ = 0;
  std::unique_ptr<BlockProperties> properties_;
  std::string block_name_ = "";
};

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Phys/Shape/VoxelShape.h"

class Dimension;
class BlockTextureAtlas;
class BlockModelLoader;
class GameContext;

struct BlockModel;

struct BlockProperties {
  BlockProperties();

  BlockProperties& SetIsFluid(bool is_fluid);
  BlockProperties& SetTransparency(bool is_fluid);

  bool is_fluid_ = false;
  bool is_solid_ = false;
  bool transparency_ = false;
  int light_emission = 0;
  int opacity_ = 15;
  VoxelShape shape_;
};

class Block {
 public:
  explicit Block(GameContext&);
  virtual ~Block();

  virtual void Tick(BlockPos pos, Dimension* currentWorld) = 0;

  GameContext& context_;
  BlockID id_ = 0;
  std::unique_ptr<BlockProperties> properties_;
  std::string block_name_ = "";
};

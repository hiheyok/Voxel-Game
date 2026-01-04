// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Block.h"

#include <memory>

#include "Core/GameContext/GameContext.h"

using std::make_unique;

Block::Block(GameContext& context) : context_{context} {
  properties_ = make_unique<BlockProperties>();
}
Block::~Block() = default;

BlockProperties::BlockProperties() = default;

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/EntityType.h"

class FallingBlock : public EntityType {
 public:
  FallingBlock(GameContext&);
  ~FallingBlock();

  void Tick(Entity* entity, Dimension* dimension) override;
};

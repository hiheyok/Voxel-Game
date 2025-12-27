// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/EntityTypeData.h"

class Hostile : public EntityTypeData {
 public:
  Hostile(GameContext&);
  ~Hostile();

  void Tick(Entity* entity, Dimension* dimension) override;
};

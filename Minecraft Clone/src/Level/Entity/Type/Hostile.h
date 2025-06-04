// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/EntityType.h"

class Hostile : public EntityType {
 public:
  Hostile(GameContext&);
  ~Hostile();

  void Tick(Entity* entity, Dimension* dimension) override;
};

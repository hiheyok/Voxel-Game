// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/EntityTypeData.h"

class Passive : public EntityTypeData {
 public:
  Passive(GameContext&);
  ~Passive();

  void Tick(Entity* entity, Dimension* dimension) override;
};

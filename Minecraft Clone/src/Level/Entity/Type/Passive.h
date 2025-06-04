// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/EntityType.h"

class Passive : public EntityType {
 public:
  Passive(GameContext&);
  ~Passive();

  void Tick(Entity* entity, Dimension* dimension) override;
};

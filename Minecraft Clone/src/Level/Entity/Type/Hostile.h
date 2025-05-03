// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/Type/EntityType.h"

class Dimension;

struct Entity;

class Hostile : public EntityType {
  void Tick(Entity* entity, Dimension* dimension) override;
};

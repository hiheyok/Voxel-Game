// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"
#include "Level/Entity/Properties/EntityProperties.h"

class Dimension;

struct Entity {
  Entity();
  virtual ~Entity();
  void Tick(Dimension* dimension);

  EntityProperty
      properties_;  // Provide properties such as position, acceleration, etc.
  bool is_dirty_ = true;
};

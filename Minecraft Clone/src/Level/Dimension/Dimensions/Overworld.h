// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Dimension/Dimension.h"

class OverworldDimension : public Dimension {
 public:
  OverworldDimension(DimensionProperties properties)
      : Dimension(properties, g_generators.MOUNTAINS) {}

  void Tick() override {}
};

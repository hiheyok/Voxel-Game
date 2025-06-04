// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Dimension/Dimension.h"

class OverworldDimension : public Dimension {
 public:
  OverworldDimension(GameContext&, DimensionProperties properties);

  void Tick() override;
};

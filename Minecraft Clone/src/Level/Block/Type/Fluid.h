// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"
#include "Level/Block/Block.h"

struct FluidProperties {
  int spread_rate_ = 1;  // Ticks for fluid to spread
};

struct Fluid : Block {
  Fluid(GameContext&, int spread_rate);
  FluidProperties fluid_properties_;
  // Use custom block model for fluids
  void InitializeBlockModel(ModelLoader& modelLoader) override;
  void Tick(BlockPos pos, Dimension* currentWorld) override;
};

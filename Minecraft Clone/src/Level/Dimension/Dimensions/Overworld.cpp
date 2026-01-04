// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Dimension/Dimensions/Overworld.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Dimension/DimensionProperties.h"

OverworldDimension::OverworldDimension(GameContext& context,
                                       DimensionProperties properties)
    : Dimension{context, properties, context.generators_->MOUNTAINS} {}

void OverworldDimension::Tick() {}

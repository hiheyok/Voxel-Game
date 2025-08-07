// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Dimension/Dimensions/Overworld.h"

#include "Core/GameContext/GameContext.h"
#include "Level/TerrainGeneration/Generators/Generators.h"

OverworldDimension::OverworldDimension(GameContext& context,
                                       DimensionProperties properties)
    : Dimension{context, properties, context.generators_->MOUNTAINS} {}

void OverworldDimension::Tick() {}

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Dimension/Dimensions/Overworld.h"

#include "Core/GameContext/GameContext.h"
#include "Level/TerrainGeneration/Generators/Generators.h"

OverworldDimension::OverworldDimension(GameContext& game_context,
                                       DimensionProperties properties)
    : Dimension{game_context, properties, game_context.generators_->MOUNTAINS} {
}

void OverworldDimension::Tick() {}

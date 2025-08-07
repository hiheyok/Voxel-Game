// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Entity/Type/Hostile.h"

#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entity.h"

Hostile::Hostile(GameContext& context) : EntityType{context} {}
Hostile::~Hostile() = default;

void Hostile::Tick(Entity* entity, Dimension* dimension) {
  (void)entity;
  (void)dimension;
}

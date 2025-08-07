// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Entity/Type/Passive.h"

#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entity.h"

Passive::Passive(GameContext& context) : EntityType{context} {}
Passive::~Passive() = default;

void Passive::Tick(Entity* entity, Dimension* dimension) {
  (void)entity;
  (void)dimension;
}

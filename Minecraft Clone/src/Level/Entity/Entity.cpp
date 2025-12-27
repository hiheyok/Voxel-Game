// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Entity/Entity.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Entity/Entities.h"

Entity::Entity() = default;
Entity::~Entity() = default;

void Entity::Tick(Dimension* dimension) {
  dimension->context_.entities_list_
      ->entity_type_list_[static_cast<int>(properties_.type_)]
      ->Tick(this, dimension);
}

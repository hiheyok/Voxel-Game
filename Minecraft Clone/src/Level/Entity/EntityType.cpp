// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Entity/EntityType.h"

EntityType::EntityType(GameContext& game_context)
    : game_context_{game_context} {}
EntityType::~EntityType() = default;
void EntityType::ChangeHitboxSize(glm::vec3 size) { hitbox_.size_ = size; }

void EntityType::ChangeHitboxSize(float x, float y, float z) {
  hitbox_.size_ = glm::vec3(x, y, z);
}

AABB& EntityType::GetHitbox() { return hitbox_; }
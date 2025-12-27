// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Entity/EntityTypeData.h"

EntityTypeData::EntityTypeData(GameContext& context) : context_{context} {}
EntityTypeData::~EntityTypeData() = default;
void EntityTypeData::ChangeHitboxSize(glm::vec3 size) { hitbox_.max_ = size; }

void EntityTypeData::ChangeHitboxSize(float x, float y, float z) {
  hitbox_.max_ = {x, y, z};
}

AABB& EntityTypeData::GetHitbox() { return hitbox_; }
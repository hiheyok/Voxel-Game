// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Assets/Types/EntityModel.h"
#include "Core/Typenames.h"
#include "Level/ECS/EntityType.h"
#include "Level/Phys/Shape/AABB.h"

class Dimension;
class GameContext;

struct Entity;

class EntityTypeData {
 public:
  explicit EntityTypeData(GameContext&);
  virtual ~EntityTypeData();

  void ChangeHitboxSize(glm::vec3 size);
  void ChangeHitboxSize(float x, float y, float z);
  AABB& GetHitbox();

  virtual void Tick(Entity* entity, Dimension* dimension) = 0;

  GameContext& context_;
  EntityType id_ = EntityType::kInvalidEntity;
  std::string entity_name_;

 private:
  AABB hitbox_;
};

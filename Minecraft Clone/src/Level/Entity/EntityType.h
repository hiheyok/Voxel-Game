// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Core/Typenames.h"
#include "Level/Entity/Collusion/Hitbox.h"
#include "RenderEngine/EntityRender/Models/EntityModel.h"
#include "RenderEngine/OpenGL/Texture/Types/Texture2D.h"

class Dimension;
class GameContext;

struct Entity;

class EntityType {
 public:
  explicit EntityType(GameContext&);
  virtual ~EntityType();

  void ChangeHitboxSize(glm::vec3 size);
  void ChangeHitboxSize(float x, float y, float z);
  AABB& GetHitbox();

  virtual void Tick(Entity* entity, Dimension* dimension) = 0;

  GameContext& context_;
  EntityTypeID id_ = 0;
  EntityModel render_model_;
  std::unique_ptr<Texture2D> texture_;
  std::string entity_name_;

 private:
  AABB hitbox_;
};

// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Level/ECS/EntityType.h"
#include "Level/Entity/EntityTypeData.h"

class GameContext;

enum EntityTypeEnums { ENTITY_PASSIVE, ENTITY_HOSTILE, ENTITY_FALLING_BLOCK };

class EntitiesList {
 private:
  int entity_type_count_ = 0;
  FastHashMap<std::string, EntityType> entity_name_id_;

 public:
  explicit EntitiesList(GameContext&);

  void InitializeModels();
  void Initialize();
  EntityType RegisterEntity(const std::string& entity_name,
                            EntityTypeEnums type, EntityType entity_type);
  EntityTypeData* GetEntity(EntityType id);

  GameContext& context_;
  std::vector<EntityTypeData*> entity_type_list_;

  EntityType HUMAN =
      RegisterEntity("human", ENTITY_PASSIVE, EntityType::kHuman);
  EntityType PLAYER =
      RegisterEntity("player", ENTITY_PASSIVE, EntityType::kPlayer);
  EntityType ZOMBIE =
      RegisterEntity("zombie", ENTITY_HOSTILE, EntityType::kZombie);
  EntityType SAND_GRAVITY_BLOCK =
      RegisterEntity("sand", ENTITY_FALLING_BLOCK, EntityType::kFallingBlock);
};

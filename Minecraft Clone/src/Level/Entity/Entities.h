// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Level/Entity/EntityType.h"

class GameContext;

enum EntityTypeEnums { ENTITY_PASSIVE, ENTITY_HOSTILE, ENTITY_FALLING_BLOCK };

class EntitiesList {
 private:
  int entity_type_count_ = 0;
  FastHashMap<std::string, EntityTypeID> entity_name_id_;

 public:
  explicit EntitiesList(GameContext&);

  void InitializeModels();
  void Initialize();
  EntityTypeID RegisterEntity(std::string entityName, EntityTypeEnums type_);
  EntityType* GetEntity(EntityTypeID id);

  GameContext& game_context_;
  std::vector<EntityType*> entity_type_list_;

  EntityTypeID HUMAN = RegisterEntity("human", ENTITY_PASSIVE);
  EntityTypeID PLAYER = RegisterEntity("player", ENTITY_PASSIVE);
  EntityTypeID ZOMBIE = RegisterEntity("zombie", ENTITY_HOSTILE);
  EntityTypeID SAND_GRAVITY_BLOCK =
      RegisterEntity("sand", ENTITY_FALLING_BLOCK);
};


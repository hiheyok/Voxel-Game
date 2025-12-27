// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include "Level/Entity/Entity.h"
#include "Level/Item/Inventory/EntityInventory.h"
#include "Level/ECS/EntityType.h"

class Player : public Entity {
 public:
  EntityInventory entity_inventory_;

  Player() { 
    properties_.type_ = EntityType::kPlayer;
    properties_.is_chunk_loader_ = true; 
  }

 private:
};

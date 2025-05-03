// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "Core/Typenames.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Item/ItemStack.h"

namespace EntityUpdatePacket {
enum PacketType {
  ENTITY_SPAWN,
  ENTITY_MOVE,
  ENTITY_INVENTORY_UPDATE,
  ENTITY_DESPAWN
};

struct EntitySpawn {
  EntitySpawn() = default;
  EntityProperty properties_;
};

struct EntityMove {
  EntityMove() = default;
  EntityProperty properties_;
};

struct EntityInventoryUpdate {
  EntityInventoryUpdate() = default;
  EntityUUID uuid;
  ItemStack item_;
  int slot_;
};

struct EntityDespawn {
  EntityDespawn() = default;
  EntityUUID uuid_;
};
}  // namespace EntityUpdatePacket

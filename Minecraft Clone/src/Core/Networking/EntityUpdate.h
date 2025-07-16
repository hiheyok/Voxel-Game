// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "Core/Typenames.h"
#include "Level/Entity/Properties/EntityProperties.h"
#include "Level/Item/ItemStack.h"

namespace EntityUpdatePacket {
enum class PacketType {
  ENTITY_SPAWN,
  ENTITY_MOVE,
  ENTITY_INVENTORY_UPDATE,
  ENTITY_DESPAWN
};

struct EntityPacketBase {};

struct EntitySpawn : EntityPacketBase {
  EntitySpawn() = default;
  EntityProperty properties_;
  static constexpr PacketType kType = PacketType::ENTITY_SPAWN;
};

struct EntityMove : EntityPacketBase {
  EntityMove() = default;
  EntityProperty properties_;
  static constexpr PacketType kType = PacketType::ENTITY_MOVE;
};

struct EntityInventoryUpdate : EntityPacketBase {
  EntityInventoryUpdate() = default;
  EntityUUID uuid;
  ItemStack item_;
  int slot_;
  static constexpr PacketType kType = PacketType::ENTITY_INVENTORY_UPDATE;
};

struct EntityDespawn : EntityPacketBase {
  EntityDespawn() = default;
  EntityUUID uuid_;
  static constexpr PacketType kType = PacketType::ENTITY_DESPAWN;
};
}  // namespace EntityUpdatePacket

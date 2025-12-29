// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"
#include "Level/ECS/Components/TransformComponent.h"
#include "Level/ECS/EntityType.h"

namespace ECSUpdatePacket {

enum class PacketType { kTransformBatch, kEntityRemoval };

// Base class for type checking
struct ECSPacketBase {};

// Individual system batches
struct TransformData {
  EntityUUID uuid_;
  EntityType type_;
  TransformComponent component_;  // Embed instead of duplicate fields
};

struct TransformBatch : ECSPacketBase {
  std::vector<TransformData> transforms_;
  static constexpr PacketType kType = PacketType::kTransformBatch;
};

// Entity removal batch - sent when entities are destroyed
struct EntityRemovalBatch : ECSPacketBase {
  std::vector<EntityUUID> removed_uuids_;
  static constexpr PacketType kType = PacketType::kEntityRemoval;
};

// Variant wrapper for all ECS packets (future-proof for additional systems)
struct ECSUpdate {
  PacketType type_;
  std::variant<TransformBatch, EntityRemovalBatch, std::monostate> packet_;

  template <typename T>
    requires std::is_base_of_v<ECSPacketBase, std::decay_t<T>>
  ECSUpdate(T&& packet) : packet_{std::forward<T>(packet)} {
    type_ = std::decay_t<T>::kType;
  }

  ECSUpdate() : type_(PacketType{}), packet_(std::monostate{}) {}
};

}  // namespace ECSUpdatePacket

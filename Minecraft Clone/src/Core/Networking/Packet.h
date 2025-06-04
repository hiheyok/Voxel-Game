// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <variant>

#include "Core/Networking/BlockUpdate.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/EntityUpdate.h"
#include "Core/Networking/PlayerAction.h"

namespace Packet {

struct PlayerAction {
  PlayerPacket::PacketType type_;
  std::variant<PlayerPacket::PlayerDestroyBlock, PlayerPacket::PlayerGetItem,
               PlayerPacket::PlayerMove, PlayerPacket::PlayerPlaceBlock,
               std::monostate>
      packet_;

  template <class T>
  PlayerAction(const T& packet) {
    packet_ = packet;
    if constexpr (std::is_same_v<T, PlayerPacket::PlayerDestroyBlock>) {
      type_ = PlayerPacket::DESTROY_BLOCK;
    } else if constexpr (std::is_same_v<T, PlayerPacket::PlayerGetItem>) {
      type_ = PlayerPacket::GET_ITEM;
    } else if constexpr (std::is_same_v<T, PlayerPacket::PlayerMove>) {
      type_ = PlayerPacket::MOVE;
    } else if constexpr (std::is_same_v<T, PlayerPacket::PlayerPlaceBlock>) {
      type_ = PlayerPacket::PLACE_BLOCK;
    }
  }
};

struct EntityUpdate {
  EntityUpdatePacket::PacketType type_;
  std::variant<EntityUpdatePacket::EntityDespawn,
               EntityUpdatePacket::EntityInventoryUpdate,
               EntityUpdatePacket::EntityMove, EntityUpdatePacket::EntitySpawn,
               std::monostate>
      packet_;

  template <class T>
  EntityUpdate(const T& packet) {
    packet_ = packet;
    if constexpr (std::is_same_v<T, EntityUpdatePacket::EntityDespawn>) {
      type_ = EntityUpdatePacket::ENTITY_DESPAWN;
    } else if constexpr (std::is_same_v<
                             T, EntityUpdatePacket::EntityInventoryUpdate>) {
      type_ = EntityUpdatePacket::ENTITY_INVENTORY_UPDATE;
    } else if constexpr (std::is_same_v<T, EntityUpdatePacket::EntityMove>) {
      type_ = EntityUpdatePacket::ENTITY_MOVE;
    } else if constexpr (std::is_same_v<T, EntityUpdatePacket::EntitySpawn>) {
      type_ = EntityUpdatePacket::ENTITY_SPAWN;
    }
  }
};

struct BlockUpdate {
  std::variant<BlockUpdatePacket::BlockUpdate, std::monostate> packet_;
};

struct ChunkUpdateData {
  std::variant<ChunkUpdatePacket::AddChunk, ChunkUpdatePacket::DeleteChunk,
               ChunkUpdatePacket::LightUpdate, std::monostate>
      packet_;
  ChunkUpdatePacket::PacketType type_;

  template <class T>
  ChunkUpdateData(const T& packet)
      : packet_(packet), type_([&]() {
          if constexpr (std::is_same_v<T, ChunkUpdatePacket::AddChunk>)
            return ChunkUpdatePacket::ADD_CHUNK;
          else if constexpr (std::is_same_v<T, ChunkUpdatePacket::DeleteChunk>)
            return ChunkUpdatePacket::DELETE_CHUNK;
          else if constexpr (std::is_same_v<T, ChunkUpdatePacket::LightUpdate>)
            return ChunkUpdatePacket::LIGHT_UPDATE;
          else
            return ChunkUpdatePacket::PacketType{};
        }()) {}
};
}  // namespace Packet

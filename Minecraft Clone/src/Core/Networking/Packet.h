// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <concepts>
#include <type_traits>
#include <variant>

#include "Core/Networking/BlockUpdate.h"
#include "Core/Networking/ChunkUpdate.h"
#include "Core/Networking/EntityUpdate.h"
#include "Core/Networking/ECSUpdate.h"
#include "Core/Networking/PlayerAction.h"

namespace Packet {

template <typename T>
concept IsPlayerUpdate =
    std::is_base_of_v<PlayerPacket::PlayerPacketBase, std::decay_t<T>>;

template <typename T>
concept IsBlockUpdate =
    std::is_base_of_v<BlockUpdatePacket::BlockUpdateBase, std::decay_t<T>>;

template <typename T>
concept IsChunkUpdate =
    std::is_base_of_v<ChunkUpdatePacket::ChunkPacketBase, std::decay_t<T>>;

template <typename T>
concept IsEntityUpdate =
    std::is_base_of_v<EntityUpdatePacket::EntityPacketBase, std::decay_t<T>>;

struct PlayerAction {
  PlayerPacket::PacketType type_;
  std::variant<PlayerPacket::PlayerDestroyBlock, PlayerPacket::PlayerGetItem,
               PlayerPacket::PlayerMove, PlayerPacket::PlayerPlaceBlock,
               std::monostate>
      packet_;

  template <IsPlayerUpdate T>
  PlayerAction(T&& packet) : packet_{std::forward<T>(packet)} {
    type_ = std::decay_t<T>::kType;
  }

  PlayerAction()
      : type_(PlayerPacket::PacketType{}), packet_(std::monostate{}) {}
};

struct EntityUpdate {
  EntityUpdatePacket::PacketType type_;
  std::variant<EntityUpdatePacket::EntityDespawn,
               EntityUpdatePacket::EntityInventoryUpdate,
               EntityUpdatePacket::EntityMove, EntityUpdatePacket::EntitySpawn,
               std::monostate>
      packet_;

  template <IsEntityUpdate T>
  EntityUpdate(T&& packet) : packet_{std::forward<T>(packet)} {
    type_ = std::decay_t<T>::kType;
  }

  EntityUpdate()
      : type_(EntityUpdatePacket::PacketType{}), packet_(std::monostate{}) {}
};

struct BlockUpdate {
  std::variant<BlockUpdatePacket::BlockUpdate,
               BlockUpdatePacket::BlockMultiUpdate, std::monostate>
      packet_;
  BlockUpdatePacket::PacketType type_;

  template <IsBlockUpdate T>
  BlockUpdate(T&& packet) : packet_(std::forward<T>(packet)) {
    type_ = std::decay_t<T>::kType;
  }

  BlockUpdate()
      : type_(BlockUpdatePacket::PacketType::kNull),
        packet_(std::monostate{}) {}
};

struct ChunkUpdateData {
  std::variant<ChunkUpdatePacket::AddChunk, ChunkUpdatePacket::DeleteChunk,
               ChunkUpdatePacket::LightUpdate, std::monostate>
      packet_;
  ChunkUpdatePacket::PacketType type_;

  template <IsChunkUpdate T>
  ChunkUpdateData(T&& packet) : packet_(std::forward<T>(packet)) {
    type_ = std::decay_t<T>::kType;
  }

  ChunkUpdateData()
      : type_(ChunkUpdatePacket::PacketType{}), packet_(std::monostate{}) {}
};
}  // namespace Packet

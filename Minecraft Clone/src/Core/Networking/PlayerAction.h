// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <glm/vec3.hpp>

#include "Core/Typenames.h"
#include "Level/Item/ItemStack.h"

namespace PlayerPacket {

enum class PacketType { GET_ITEM, DESTROY_BLOCK, PLACE_BLOCK, MOVE };

struct PlayerPacketBase {};

struct PlayerGetItem : PlayerPacketBase {
  ItemStack item_;
  int slot_;
  static constexpr PacketType kType = PacketType::GET_ITEM;
};

struct PlayerDestroyBlock : PlayerPacketBase {
  BlockPos pos_;
  static constexpr PacketType kType = PacketType::DESTROY_BLOCK;
};

struct PlayerPlaceBlock : PlayerPacketBase {
  BlockPos pos_;
  BlockID block_;
  static constexpr PacketType kType = PacketType::PLACE_BLOCK;
};

struct PlayerMove : PlayerPacketBase {
  glm::vec3 pos_, vel_, acc_;
  static constexpr PacketType kType = PacketType::MOVE;
};
}  // namespace PlayerPacket

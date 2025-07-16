// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Core/Typenames.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Light/LightStorage.h"

namespace ChunkUpdatePacket {

enum class PacketType { ADD_CHUNK, LIGHT_UPDATE, DELETE_CHUNK };

struct ChunkPacketBase {};

struct AddChunk : ChunkPacketBase {
  ChunkRawData chunk_;
  static constexpr PacketType kType = PacketType::ADD_CHUNK;
};

struct LightUpdate : ChunkPacketBase {
  LightStorage sky_light_;
  LightStorage block_light_;
  static constexpr PacketType kType = PacketType::LIGHT_UPDATE;
};

struct DeleteChunk : ChunkPacketBase {
  ChunkPos pos_;
  static constexpr PacketType kType = PacketType::DELETE_CHUNK;
};
}  // namespace ChunkUpdatePacket

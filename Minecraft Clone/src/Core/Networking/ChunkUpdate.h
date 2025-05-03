// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "Core/Typenames.h"
#include "Level/Chunk/ChunkRawData.h"
#include "Level/Light/LightStorage.h"

namespace ChunkUpdatePacket {

enum PacketType { ADD_CHUNK, LIGHT_UPDATE, DELETE_CHUNK };

struct AddChunk {
  ChunkRawData chunk_;
};

struct LightUpdate {
  LightStorage light_;
};

struct DeleteChunk {
  ChunkPos pos_;
};
}  // namespace ChunkUpdatePacket

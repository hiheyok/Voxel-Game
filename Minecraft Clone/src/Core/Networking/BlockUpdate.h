// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <utility>
#include <vector>

#include "Core/Typenames.h"

namespace BlockUpdatePacket {

enum class PacketType : uint8_t {
  kNull = 0,
  kBlockUpdate = 1,
  kBlockMultiUpdate = 2
};

struct BlockUpdateBase {};

struct BlockUpdate : BlockUpdateBase {
  BlockID block_;
  BlockPos pos_;
  static constexpr PacketType kType = PacketType::kBlockUpdate;
};

struct BlockMultiUpdate : BlockUpdateBase {
  std::vector<std::pair<BlockPos, BlockID>> updates_;
  ChunkPos chunk_pos_;
  static constexpr PacketType kType = PacketType::kBlockMultiUpdate;
};

}  // namespace BlockUpdatePacket

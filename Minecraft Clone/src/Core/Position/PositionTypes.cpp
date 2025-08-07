// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Core/Position/PositionTypes.h"

#include "Core/Typenames.h"

static constexpr int kBlockToRegionLog2 = kChunkDimLog2 + kRegionDimLog2;

BlockPos ChunkPos::GetBlockPosOffset() const noexcept {
  return BlockPos{x, y, z} * kChunkDim;
}

RegionPos ChunkPos::ToRegionPos() const noexcept {
  return RegionPos{x, y, z} >> kRegionDimLog2;
}

size_t ChunkPos::GetIndex() const noexcept {
  static constexpr uint32_t mask = kRegionDim - 1;

  return kRegionSize2D * (x & mask) | kRegionDim * (y & mask) | (z & mask);
}

RegionPos BlockPos::ToRegionPos() const noexcept {
  return RegionPos{x, y, z} >> kBlockToRegionLog2;
}

ChunkPos BlockPos::ToChunkPos() const noexcept {
  return ChunkPos{x, y, z} >> kChunkDimLog2;
}

size_t BlockPos::GetIndex() const noexcept {
  static constexpr uint32_t mask = kChunkDim - 1;

  return kChunkSize2D * (x & mask) | kChunkDim * (y & mask) | (z & mask);
}

BlockPos BlockPos::GetLocalPos() const noexcept {
  static constexpr uint32_t mask = kChunkDim - 1;

  return {x & mask, y & mask, z & mask};
}

bool BlockPos::IsInSameChunk(BlockPos other) const noexcept {
  static constexpr uint32_t mask = ~static_cast<uint32_t>(kChunkDim - 1);
  return (x & mask) == (other.x & mask) && (y & mask) == (other.y & mask) &&
         (z & mask) == (other.z & mask);
}

#include "Core/Position/PositionTypes.h"

#include "Core/Typenames.h"

static constexpr int kBlockToRegionLog2 = kChunkDimLog2 + kRegionDimLog2;

RegionPos ChunkPos::ToRegionPos() const noexcept {
  return RegionPos{x >> kRegionDimLog2, y >> kRegionDimLog2,
                   z >> kRegionDimLog2};
}

size_t ChunkPos::GetIndex() const noexcept {
  static constexpr uint32_t mask = kRegionDim - 1;

  return kRegionSize2D * (x & mask) + kRegionDim * (y & mask) + (z & mask);
}

RegionPos BlockPos::ToRegionPos() const noexcept {
  return RegionPos{x >> kBlockToRegionLog2, y >> kBlockToRegionLog2,
                   z >> kBlockToRegionLog2};
}

ChunkPos BlockPos::ToChunkPos() const noexcept {
  return ChunkPos{x >> kChunkDimLog2, y >> kChunkDimLog2, z >> kChunkDimLog2};
}

size_t BlockPos::GetIndex() const noexcept {
  static constexpr uint32_t mask = kChunkDim - 1;

  return kChunkSize2D * (x & mask) + kChunkDim * (y & mask) + (z & mask);
}

BlockPos BlockPos::GetLocalPos() const noexcept {
  static constexpr uint32_t mask = kChunkDim - 1;

  return BlockPos{x & mask, y & mask, z & mask};
}

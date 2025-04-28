#include "Core/Position/PositionTypes.h"

#include "Core/Typenames.h"

static constexpr int kBlockToRegion = kChunkDim * kRegionDim;

ChunkPos::operator RegionPos() const noexcept {
    return RegionPos{x, y, z} / kRegionDim;
}

size_t ChunkPos::GetIndex() const noexcept {
    static constexpr uint32_t mask = kRegionDim - 1;

    return kRegionSize2D * (x & mask) + kRegionDim * (y & mask) + (z & mask);
}

BlockPos::operator RegionPos() const noexcept {
    return RegionPos{x, y, z} / kBlockToRegion;
}

BlockPos::operator ChunkPos() const noexcept {
    return ChunkPos{x, y, z} / kChunkDim;
}

size_t BlockPos::GetIndex() const noexcept {
    static constexpr uint32_t mask = kChunkDim - 1;

    return kChunkSize2D * (x & mask) + kChunkDim * (y & mask) + (z & mask);
}

BlockPos BlockPos::GetLocalPos() const noexcept {
    static constexpr uint32_t mask = kChunkDim - 1;

    return BlockPos{x & mask, y & mask, z & mask};
}
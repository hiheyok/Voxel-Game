
#ifndef CORE_POSITION_POSITION_TYPES_H
#define CORE_POSITION_POSITION_TYPES_H

#include "Core/Position/Position.h"

class ChunkPos;
class RegionPos;
class BlockPos;

class RegionPos final : public Position<RegionPos> {
 public:
  using Position<RegionPos>::Position;
};

class ChunkPos final : public Position<ChunkPos> {
 public:
  using Position<ChunkPos>::Position;

  [[nodiscard]] RegionPos ToRegionPos() const noexcept;
  [[nodiscard]] size_t GetIndex() const noexcept;
};

class BlockPos final : public Position<BlockPos> {
 public:
  using Position<BlockPos>::Position;

  [[nodiscard]] RegionPos ToRegionPos() const noexcept;
  [[nodiscard]] ChunkPos ToChunkPos() const noexcept;

  [[nodiscard]] size_t GetIndex() const noexcept;
  [[nodiscard]] BlockPos GetLocalPos() const noexcept;
};

#endif

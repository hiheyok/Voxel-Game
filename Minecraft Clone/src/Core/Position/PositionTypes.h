// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

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

  [[nodiscard]] BlockPos GetBlockPosOffset() const noexcept;
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

static_assert(std::is_trivially_copyable_v<RegionPos>);
static_assert(std::is_trivially_copyable_v<ChunkPos>);
static_assert(std::is_trivially_copyable_v<BlockPos>);

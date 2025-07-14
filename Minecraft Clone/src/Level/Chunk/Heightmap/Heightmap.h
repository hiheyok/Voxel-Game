// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <array>

#include "Core/Typenames.h"
/*

heightmap for chunk column 32 * 16 = 512 blocks columns 9 bits per column

16 x 16 * 9 = 2304 bits = 36 uint64_t
*/

class HeightMap {
 public:
  HeightMap();
  ~HeightMap();

  void Edit(int x, int z, int height) noexcept;

  int8_t Get(int x, int z) const noexcept;

  void Clear();

  static constexpr int8_t kNoBlocks = -1;
  static constexpr int8_t kColumnFull = kChunkDim;

 private:
  std::array<int8_t, kChunkSize2D> data_;
};

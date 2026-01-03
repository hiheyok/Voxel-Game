// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <vector>

#include "Core/Typenames.h"
#include "Level/Block/Blocks.h"
#include "Utils/Containers/NBitVector.h"
/*
 * The palette class is used to store the block data in a compressed state in a
 * chunk Each chunk has up to 4096 unique blocks so it attempts to store it in
 * up to 12 bits per blocks
 */

// TODO(hiheyok): for a large amount of unique block, use stack / hashmap
class Palette {
 public:
  explicit Palette();
  ~Palette();
  Palette(Palette&&) noexcept;
  Palette(const Palette&) noexcept;
  Palette& operator=(const Palette&);
  Palette& operator=(Palette&&) noexcept;

  BlockID GetBlock(BlockPos pos) const noexcept;
  void SetBlock(BlockID block, BlockPos pos);

  std::array<BlockID, kChunkSize3D> UnpackAll() const;

  /*
   Important: This function unpacks slice in this specific format
   - x: yz
   - y: zy
   - z: xy
  */
  std::array<BlockID, kChunkSize2D> UnpackSlice(int axis, int slice_idx) const;

 private:
  using PaletteIndex = uint16_t;
  using StorageBit = uint64_t;

  static constexpr size_t GetBitWidth(uint32_t n) noexcept {
    size_t bit_width = std::bit_width(n);
    return std::max(bit_width, kMinBitWidth);
  }

  void Shrink();
  void Grow();
  void Resize();

  PaletteIndex GetOrAddPaletteIndex(BlockID block);

  static constexpr size_t kMinBitWidth = 1;  // Minimum bits
  static constexpr size_t kMaxBitWidth = 12;
  // Only shrink when empty slots exceed 25% of capacity
  static constexpr float kLazyShrinkThreshold = 0.25f;

  size_t current_bit_width_ = kMinBitWidth;
  size_t unique_blocks_count_ = 1;  // Initialize with only air blocks

  NBitVector data_;

  // BlockID -> Block, int_16 -> num of that block
  std::vector<BlockID> palette_entries_id_;
  std::vector<int16_t> palette_entries_count_;

  // Free list of reusable palette indices (replaces linear scan)
  std::vector<PaletteIndex> free_slots_;
};

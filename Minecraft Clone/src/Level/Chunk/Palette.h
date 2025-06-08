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

  BlockID GetBlock(BlockPos pos) const;
  BlockID GetBlockUnsafe(BlockPos pos) const noexcept;

  void SetBlock(BlockID block, BlockPos pos);
  void SetBlockUnsafe(BlockID block, BlockPos pos);

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

  static constexpr int GetBitWidth(uint32_t n) noexcept {
    int bitWidth = std::bit_width(n);
    return std::max(bitWidth, kMinBitWidth);
  }

  void Shrink();
  void Grow();
  void Resize();

  PaletteIndex GetOrAddPaletteIndex(BlockID block);

  static constexpr int kMinBitWidth = 1;  // Minimum bits
  static constexpr int kMaxBitWidth = 12;

  int current_bit_width_ = kMinBitWidth;
  int unique_blocks_count_ = 1;  // Initialize with only air blocks
  int empty_slot_counter_ = 0;   // This counts the amount of empty entries
                                 // (count == 0) in palette_entries_

  NBitVector<StorageBit> data_;

  // BlockID -> Block, int_16 -> num of that block
  std::vector<BlockID> palette_entries_id_;
  std::vector<int16_t> palette_entries_count_;
};

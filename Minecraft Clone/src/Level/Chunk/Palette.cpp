// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Chunk/Palette.h"

#include "Core/GameContext/GameContext.h"
#include "Level/Block/Blocks.h"

Palette::Palette()
    : current_bit_width_{kMinBitWidth},
      data_{kChunkSize3D, static_cast<size_t>(current_bit_width_)} {
  palette_entries_id_.emplace_back(0);
  palette_entries_count_.emplace_back(static_cast<uint16_t>(kChunkSize3D));
  // TODO(hiheyok): Change this default state from 0 to blocks.AIR later
}

Palette::~Palette() = default;
Palette::Palette(Palette&&) noexcept = default;
Palette::Palette(const Palette&) noexcept = default;
Palette& Palette::operator=(const Palette&) = default;
Palette& Palette::operator=(Palette&&) noexcept = default;

void Palette::Shrink() {
  int newBitWidth = GetBitWidth(unique_blocks_count_);
  NBitVector<StorageBit> newData(kChunkSize3D, newBitWidth);

  // Repack

  std::vector<BlockID> new_block_to_palette_id_(unique_blocks_count_);
  std::vector<int16_t> new_block_to_palette_count_(unique_blocks_count_);
  std::vector<PaletteIndex> new_palette_index(palette_entries_id_.size(), 0);

  // 2 pointers

  PaletteIndex curr = 0;
  for (size_t i = 0; i < palette_entries_id_.size(); ++i) {
    if (palette_entries_count_[i] == 0) continue;
    new_block_to_palette_id_[curr] = palette_entries_id_[i];
    new_block_to_palette_count_[curr] = palette_entries_count_[i];
    new_palette_index[i] = curr;
    curr++;
  }

  for (int i = 0; i < kChunkSize3D; ++i) {
    PaletteIndex currVal = static_cast<PaletteIndex>(data_.Get(i));
    PaletteIndex newVal = new_palette_index[currVal];
    newData.Set(i, newVal);
  }

  data_ = std::move(newData);
  current_bit_width_ = newBitWidth;

  // refactor palette block index
  palette_entries_id_ = std::move(new_block_to_palette_id_);
  palette_entries_count_ = std::move(new_block_to_palette_count_);
  empty_slot_counter_ = 0;
}

void Palette::Grow() {
  int newBitWidth = GetBitWidth(unique_blocks_count_);
  NBitVector<StorageBit> newData(kChunkSize3D, newBitWidth);
  current_bit_width_ = newBitWidth;
  for (int i = 0; i < kChunkSize3D; ++i) {
    newData.Set(i, data_.Get(i));
  }

  data_ = std::move(newData);
}

void Palette::Resize() {
  int newBitWidth = GetBitWidth(unique_blocks_count_);
  // Plus 1 to avoid frequent resizing
  if (newBitWidth == current_bit_width_ ||
      newBitWidth + 1 == current_bit_width_)
    return;

  if (newBitWidth + 1 < current_bit_width_) {
    Shrink();
  } else {
    Grow();
  }
}

Palette::PaletteIndex Palette::GetOrAddPaletteIndex(BlockID block) {
  PaletteIndex firstZero = 0;
  bool foundZero = false;
  for (PaletteIndex i = 0; i < palette_entries_id_.size(); i++) {
    if (palette_entries_id_[i] == block) {
      return i;
    }
    if (palette_entries_count_[i] == 0) {
      foundZero = true;
      firstZero = i;
    }
  }

  if (foundZero) {
    palette_entries_id_[firstZero] = block;
    return firstZero;
  }

  palette_entries_id_.emplace_back(block);
  palette_entries_count_.emplace_back(0);
  Resize();
  PaletteIndex newIndex =
      static_cast<PaletteIndex>(palette_entries_id_.size() - 1);

  return newIndex;
}

BlockID Palette::GetBlock(BlockPos pos) const {
  if ((pos.x | pos.y | pos.z) >> kChunkDimLog2) {
    throw std::out_of_range("Palette::GetBlock - Invalid palette index");
  }

  return GetBlockUnsafe(pos);
}

BlockID Palette::GetBlockUnsafe(BlockPos pos) const noexcept {
  PaletteIndex idx = static_cast<PaletteIndex>(data_.GetUnsafe(pos.GetIndex()));
  return palette_entries_id_[idx];
}

void Palette::SetBlock(BlockID block, BlockPos pos) {
  if ((pos.x | pos.y | pos.z) >> kChunkDimLog2) {
    throw std::out_of_range("Palette::SetBlock - Invalid palette index");
  }

  SetBlockUnsafe(block, pos);
}

void Palette::SetBlockUnsafe(BlockID block, BlockPos pos) {
  // Look at original block
  PaletteIndex oldPaletteIdx =
      static_cast<PaletteIndex>(data_.GetUnsafe(pos.GetIndex()));
  if (oldPaletteIdx >= palette_entries_count_.size() ||
      palette_entries_count_[oldPaletteIdx] <= 0) {
    throw std::runtime_error(
        "Palette::SetBlockUnsafe - Corrupt old palette index found in "
        "data.");
  }
  BlockID oldBlockId = palette_entries_id_[oldPaletteIdx];
  bool uniqueCountChanged = false;
  if (block == oldBlockId) {
    return;
  }

  if (--palette_entries_count_[oldPaletteIdx] == 0) {
    --unique_blocks_count_;
    ++empty_slot_counter_;
    uniqueCountChanged = !uniqueCountChanged;
  }

  PaletteIndex idx = GetOrAddPaletteIndex(block);
  if (palette_entries_count_[idx]++ == 0) {
    unique_blocks_count_++;
    --empty_slot_counter_;
    uniqueCountChanged = !uniqueCountChanged;
  }

  data_.SetUnsafe(pos.GetIndex(), idx);
  if (uniqueCountChanged) {
    Resize();
  }
}

std::array<BlockID, kChunkSize3D> Palette::UnpackAll() const {
  std::array<BlockID, kChunkSize3D> out;
  data_.UnpackAll(out.data());
  const BlockID* src = palette_entries_id_.data();
  for (auto& i : out) {
    i = src[i];
  }
  return out;
}

std::array<BlockID, kChunkSize2D> Palette::UnpackSlice(int axis,
                                                       int slice_idx) const {
  static constexpr int kStrideX = kChunkSize2D;
  static constexpr int kStrideY = kChunkDim;
  static constexpr int kStrideZ = 1;
  static constexpr int kStrides[3]{kStrideX, kStrideY, kStrideZ};

  std::array<BlockID, kChunkSize2D> out_slice;

  BlockPos pos;
  pos[axis] = slice_idx;

  int block_idx = slice_idx * kStrides[axis];  // Starting point

  int axis_u = (axis + 1) % 3;
  int axis_v = (axis + 2) % 3;

  // Cache friendly access pattern for y-axis (faster)
  if (axis == Directions<ChunkPos>::kYAxis) {
    std::swap(axis_u, axis_v);
  }

  const BlockID* src = palette_entries_id_.data();

  int write_idx = 0;
  for (int u = 0; u < kChunkDim; ++u) {
    int v_block_idx = block_idx;
    for (int v = 0; v < kChunkDim; ++v) {
      const PaletteIndex palette_idx =
          static_cast<PaletteIndex>(data_.GetUnsafe(v_block_idx));

      out_slice[write_idx++] = src[palette_idx];
      v_block_idx += kStrides[axis_v];
    }

    block_idx += kStrides[axis_u];
  }

  return out_slice;
}

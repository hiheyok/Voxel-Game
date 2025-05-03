// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>

template <size_t BitWidth, size_t NumElements, typename StorageBit>
class PackedBitArray {
 public:
  static_assert(BitWidth > 0, "BitWidth must be greater than 0.");
  static_assert(std::is_unsigned_v<StorageBit>,
                "StorageBit must be an unsigned integer type.");
  static_assert(BitWidth <= std::numeric_limits<StorageBit>::digits,
                "BitWidth cannot exceed the number of bits in StorageBit.");

  PackedBitArray() noexcept;
  PackedBitArray(const PackedBitArray&);
  PackedBitArray(PackedBitArray&&) noexcept;
  PackedBitArray& operator=(const PackedBitArray&);
  PackedBitArray& operator=(PackedBitArray&&) noexcept;
  ~PackedBitArray() noexcept;

  StorageBit Get(size_t idx) const noexcept;

  template <typename T>
  void Set(size_t idx, T val) noexcept;

 private:
  static constexpr size_t kStorageBits = sizeof(StorageBit) * 8;
  static constexpr StorageBit kAllOnes = ~(static_cast<StorageBit>(0));
  static constexpr StorageBit kAllOnesBitWidth = ~(kAllOnes << BitWidth);
  static constexpr std::array<StorageBit, kStorageBits> kOverflowTable = []() {
    std::array<StorageBit, kStorageBits> overflow_table;
    for (size_t i = 0; i < kStorageBits; ++i) {
      overflow_table[i] = (kStorageBits - i < BitWidth) *
                          (kAllOnesBitWidth >> (kStorageBits - i));
    }
    return overflow_table;
  };

  std::array<StorageBit, NumElements * BitWidth / kStorageBits + 1> data_;
};

template <size_t BitWidth, size_t NumElements, typename StorageBit>
PackedBitArray<BitWidth, NumElements, StorageBit>::PackedBitArray() noexcept =
    default;

template <size_t BitWidth, size_t NumElements, typename StorageBit>
PackedBitArray<BitWidth, NumElements, StorageBit>::~PackedBitArray() noexcept =
    default;

template <size_t BitWidth, size_t NumElements, typename StorageBit>
PackedBitArray<BitWidth, NumElements, StorageBit>::PackedBitArray(
    const PackedBitArray&) = default;

template <size_t BitWidth, size_t NumElements, typename StorageBit>
PackedBitArray<BitWidth, NumElements, StorageBit>::PackedBitArray(
    PackedBitArray&&) noexcept = default;

template <size_t BitWidth, size_t NumElements, typename StorageBit>
PackedBitArray<BitWidth, NumElements, StorageBit>&
PackedBitArray<BitWidth, NumElements, StorageBit>::operator=(
    const PackedBitArray<BitWidth, NumElements, StorageBit>&) = default;

template <size_t BitWidth, size_t NumElements, typename StorageBit>
PackedBitArray<BitWidth, NumElements, StorageBit>&
PackedBitArray<BitWidth, NumElements, StorageBit>::operator=(
    PackedBitArray<BitWidth, NumElements, StorageBit>&&) noexcept = default;

template <size_t BitWidth, size_t NumElements, typename StorageBit>
template <typename T>
void PackedBitArray<BitWidth, NumElements, StorageBit>::Set(size_t idx,
                                                            T val) noexcept {
  StorageBit data = static_cast<StorageBit>(val);

  size_t array_index = BitWidth * idx / kStorageBits;
  size_t integer_index = (BitWidth * idx) % kStorageBits;

  const StorageBit mask = kAllOnesBitWidth << integer_index;
  const StorageBit overflow_mask = kOverflowTable[integer_index];

  // Clear data first
  data_[array_index] &= ~mask;
  data_[array_index] |= data << integer_index;

  // Insert the left over if it overlaps
  // If there is no overflow, overflow is 0 so it will do nothing to the data
  data_[array_index + 1] &= ~overflow_mask;
  data_[array_index + 1] |=
      overflow_mask & (data >> (kStorageBits - integer_index));
}

template <size_t BitWidth, size_t NumElements, typename StorageBit>
StorageBit PackedBitArray<BitWidth, NumElements, StorageBit>::Get(
    size_t idx) const noexcept {
  size_t array_index = bit_width_ * idx / kStorageBits;
  size_t integer_index = (bit_width_ * idx) % kStorageBits;

  const StorageBit mask = kAllOnesBitWidth << integer_index;
  const StorageBit overflow_mask = kOverflowTable[integer_index];

  StorageBit data = (data_[array_index] & mask) >> integer_index;

  // If it overlap with the next bit extract the next bit too
  // If the overflow mask is 0, it means that there is no overflow
  // Because it is 0, it will set dataNext to 0 too and this will do nothing
  // to the data
  StorageBit dataNext = data_[array_index + 1] & overflow_mask;
  dataNext <<= (kStorageBits - integer_index);
  data |= dataNext;

  return data;
}
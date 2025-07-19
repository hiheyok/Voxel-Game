// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <array>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <cassert>

template <typename StorageBit = uint64_t>
class NBitVector {
  static_assert(std::is_unsigned_v<StorageBit>,
                "StorageBit is an unsigned type.");
  static_assert((sizeof(StorageBit) & (sizeof(StorageBit) - 1)) == 0,
                "size of StorageBit must be base 2");

 public:
  NBitVector(size_t numElements, size_t bit_width);
  NBitVector(size_t bit_width) noexcept;
  NBitVector() noexcept;
  NBitVector(const NBitVector&);
  NBitVector(NBitVector&&) noexcept;
  NBitVector& operator=(const NBitVector&);
  NBitVector& operator=(NBitVector&&) noexcept;
  ~NBitVector() noexcept;
  [[nodiscard]]
  StorageBit Get(size_t idx) const;
  [[nodiscard]]
  StorageBit GetUnsafe(size_t idx) const noexcept;

  template <typename T>
  void Append(T val);

  template <typename T>
  void Set(size_t idx, T val);
  template <typename T>
  void SetUnsafe(size_t idx, T val) noexcept;

  template <typename T>
  void UnpackAll(std::vector<T>& out) const;

  template <typename T>
  void UnpackAll(T* out) const;

 private:
  void ComputeMaskCache() noexcept;

  static constexpr size_t kStorageBits = sizeof(StorageBit) * 8;
  static constexpr StorageBit kAllOnes = ~(static_cast<StorageBit>(0));
  static constexpr size_t kStorageBitsLog2 = std::bit_width(kStorageBits) - 1;
  static constexpr size_t KStorageBitRemainderMask = kStorageBits - 1;

  std::vector<StorageBit> data_;
  size_t bit_width_ = 0;
  size_t num_elements_ = 0;

  StorageBit all_ones_bit_width_;

  std::array<StorageBit, kStorageBits> overflow_table_;
};

template <typename StorageBit>
NBitVector<StorageBit>::NBitVector() noexcept {
  ComputeMaskCache();
}

template <typename StorageBit>
NBitVector<StorageBit>::NBitVector(size_t numElements, size_t bit_width)
    : bit_width_{bit_width}, num_elements_{numElements} {
  if (bit_width > kStorageBits)
    throw std::domain_error(
        "NBitVector<StorageBit>::NBitVector - Bit width is too wide.");

  data_.resize(numElements * bit_width / kStorageBits + 1);
  all_ones_bit_width_ = ~(kAllOnes << bit_width_);
  ComputeMaskCache();
}

template <typename StorageBit>
NBitVector<StorageBit>::NBitVector(size_t bit_width) noexcept
    : bit_width_{bit_width} {
  all_ones_bit_width_ = ~(kAllOnes << bit_width_);
  ComputeMaskCache();
}

template <typename StorageBit>
NBitVector<StorageBit>::~NBitVector() noexcept = default;

template <typename StorageBit>
NBitVector<StorageBit>::NBitVector(const NBitVector&) = default;

template <typename StorageBit>
NBitVector<StorageBit>::NBitVector(NBitVector&&) noexcept = default;

template <typename StorageBit>
NBitVector<StorageBit>& NBitVector<StorageBit>::operator=(
    const NBitVector<StorageBit>&) = default;

template <typename StorageBit>
NBitVector<StorageBit>& NBitVector<StorageBit>::operator=(
    NBitVector<StorageBit>&&) noexcept = default;

template <typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::Set(size_t idx, T val) {
  if (static_cast<size_t>(val) >= (1ULL << bit_width_))
    throw std::domain_error(
        "NBitVector<StorageBit>::Set - Invalid number. Wrong size");
  if (idx >= num_elements_)
    throw std::out_of_range("NBitVector<StorageBit>::Set - Index out of range");
  SetUnsafe(idx, val);
}

template <typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::SetUnsafe(size_t idx, T val) noexcept {
  StorageBit value = static_cast<StorageBit>(val);

  size_t total_bit_offset = bit_width_ * idx;
  size_t data_idx = total_bit_offset >> kStorageBitsLog2;
  size_t bit_pos = total_bit_offset & KStorageBitRemainderMask;

  const StorageBit low_mask = all_ones_bit_width_ << bit_pos;
  data_[data_idx] = (data_[data_idx] & ~low_mask) | (value << bit_pos);

  const StorageBit overflow_mask = overflow_table_[bit_pos];
  if (overflow_mask != 0) {
    const StorageBit high_bits = value >> (kStorageBits - bit_pos);
    data_[data_idx + 1] = (data_[data_idx + 1] & ~overflow_mask) | high_bits;
  }
}

template <typename StorageBit>
StorageBit NBitVector<StorageBit>::Get(size_t idx) const {
  if (idx >= num_elements_)
    throw std::out_of_range("NBitVector<StorageBit>::Get - Index out of range");
  return GetUnsafe(idx);
}

template <typename StorageBit>
StorageBit NBitVector<StorageBit>::GetUnsafe(size_t idx) const noexcept {
  size_t total_bit_offset = bit_width_ * idx;

  size_t data_idx = total_bit_offset >> kStorageBitsLog2;
  size_t bit_pos = total_bit_offset & KStorageBitRemainderMask;

  StorageBit result = data_[data_idx] >> bit_pos;

  const StorageBit overflow_mask = overflow_table_[bit_pos];
  if (overflow_mask != 0) {
    const StorageBit high_bits = data_[data_idx + 1];
    result |= high_bits << (kStorageBits - bit_pos);
  }

  return result & all_ones_bit_width_;
}

template <typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::Append(T val) {
  if (val >= (1 << bit_width_))
    throw std::domain_error(
        "NBitVector<StorageBit>::Append - Invalid number. Wrong size");

  num_elements_++;
  if (num_elements_ * bit_width_ / kStorageBits >=
      data_.size() - 1) {  // -1 to keep an extra space of padding
    data_.push_back(static_cast<StorageBit>(0));
  }

  Set(num_elements_ - 1, val);
}

template <typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::UnpackAll(std::vector<T>& out) const {
  out.resize(num_elements_);
  UnpackAll(out.data());
}

template <typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::UnpackAll(T* out) const {
  static_assert(std::is_same_v<StorageBit, uint64_t>);
  size_t out_idx = 0;
  size_t data_idx = 0;
  size_t bit_pos = 0;

  const StorageBit* src = data_.data();

  while (out_idx < num_elements_) {
    if (bit_pos + bit_width_ < kStorageBits) [[likely]] {  // Easy acess
      out[out_idx++] = static_cast<T>((src[data_idx] >> bit_pos) & all_ones_bit_width_);
      bit_pos += bit_width_;
    } else {
      const StorageBit lower = src[data_idx] >> bit_pos;
      const StorageBit upper = src[++data_idx] << (kStorageBits - bit_pos);

      out[out_idx++] = static_cast<T>((lower | upper) & all_ones_bit_width_);
      // Combine them and store
      bit_pos = (bit_pos + bit_width_) & KStorageBitRemainderMask;
    }
  }
}

template <typename StorageBit>
void NBitVector<StorageBit>::ComputeMaskCache() noexcept {
  for (size_t i = 0; i < kStorageBits; ++i) {
    overflow_table_[i] = (kStorageBits - i < bit_width_)
                             ? (all_ones_bit_width_ >> (kStorageBits - i))
                             : 0;
  }
}

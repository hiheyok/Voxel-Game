// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <vector>

class NBitVector {
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
  uint64_t Get(size_t idx) const;
  [[nodiscard]]
  uint64_t GetUnsafe(size_t idx) const noexcept;

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
  static constexpr size_t kDataWidth = 64;
  static constexpr uint64_t kAllOnes = ~(static_cast<uint64_t>(0));
  static constexpr size_t kDataWidthLog2 = 6;
  static constexpr size_t kDataBitRemainderMask = kDataWidth - 1;

  size_t bit_width_;
  uint64_t all_one_bit_width_;

  std::vector<uint64_t> data_;
  size_t num_elements_ = 0;

  static constexpr std::array<std::array<uint64_t, kDataWidth>, kDataWidth>
      overflow_table_ = []() constexpr {
        std::array<std::array<uint64_t, kDataWidth>, kDataWidth> table{};
        for (size_t bit_width = 0; bit_width < kDataWidth; ++bit_width) {
          const size_t all_one_bit_width = ~(kAllOnes << bit_width);
          for (size_t i = 0; i < kDataWidth; ++i) {
            if (kDataWidth - i < bit_width) {
              table[bit_width][i] = all_one_bit_width >> (kDataWidth - i);
            } else {
              table[bit_width][i] = 0;
            }
          }
        }
        return table;
      }();
};

template <typename T>
void NBitVector::Set(size_t idx, T val) {
  if (static_cast<size_t>(val) >= (1ULL << bit_width_))
    throw std::domain_error("NBitVector::Set - Invalid number. Wrong size");
  if (idx >= num_elements_)
    throw std::out_of_range("NBitVector::Set - Index out of range");
  SetUnsafe(idx, val);
}

template <typename T>
void NBitVector::SetUnsafe(size_t idx, T val) noexcept {
  uint64_t value = static_cast<uint64_t>(val);

  size_t total_bit_offset = bit_width_ * idx;
  size_t data_idx = total_bit_offset >> kDataWidthLog2;
  size_t bit_pos = total_bit_offset & kDataBitRemainderMask;

  const uint64_t low_mask = all_one_bit_width_ << bit_pos;
  data_[data_idx] = (data_[data_idx] & ~low_mask) | (value << bit_pos);

  const uint64_t overflow_mask = overflow_table_[bit_width_][bit_pos];
  if (overflow_mask != 0) [[unlikely]] {
    const uint64_t high_bits = value >> (kDataWidth - bit_pos);
    data_[data_idx + 1] = (data_[data_idx + 1] & ~overflow_mask) | high_bits;
  }
}

template <typename T>
void NBitVector::Append(T val) {
  if (val >= (1 << bit_width_))
    throw std::domain_error("NBitVector::Append - Invalid number. Wrong size");

  num_elements_++;
  if (num_elements_ * bit_width_ / kDataWidth >=
      data_.size() - 1) {  // -1 to keep an extra space of padding
    data_.emplace_back(0ULL);
  }

  Set(num_elements_ - 1, val);
}

template <typename T>
void NBitVector::UnpackAll(std::vector<T>& out) const {
  out.resize(num_elements_);
  UnpackAll(out.data());
}

template <typename T>
void NBitVector::UnpackAll(T* out) const {
  const uint64_t* src = data_.data();
  const uint64_t mask = all_one_bit_width_;

#ifdef __SIZEOF_INT128__
  size_t bit_offset = 0;
  for (size_t i = 0; i < num_elements_; ++i) {
    const size_t data_idx = bit_offset >> kDataWidthLog2;
    const size_t bit_pos = bit_offset & kDataBitRemainderMask;

    unsigned __int128 chunk =
        *reinterpret_cast<const unsigned __int128*>(&src[data_idx]);
    out[i] = static_cast<T>((chunk >> bit_pos) & mask);
    bit_offset += bit_width_;
  }
#else
  // 64-bit fallback
  size_t data_idx = 0;
  size_t bit_pos = 0;

  for (size_t i = 0; i < num_elements_; ++i) {
    if (bit_pos + bit_width_ <= kDataWidth) [[likely]] {
      out[i] = static_cast<T>((src[data_idx] >> bit_pos) & mask);
    } else {
      const uint64_t lo = src[data_idx] >> bit_pos;
      const uint64_t hi = src[++data_idx] << (kDataWidth - bit_pos);
      out[i] = static_cast<T>((lo | hi) & mask);
    }
    bit_pos = (bit_pos + bit_width_) & kDataBitRemainderMask;
    data_idx += (bit_pos == 0);
  }
#endif
}

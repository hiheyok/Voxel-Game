#include "Utils/Containers/NBitVector.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <vector>

NBitVector::NBitVector() noexcept : bit_width_{1}, all_one_bit_width_{1} {}

NBitVector::NBitVector(size_t numElements, size_t bit_width)
    : bit_width_{bit_width},
      all_one_bit_width_{~(kAllOnes << bit_width)},
      num_elements_{numElements} {
  if (bit_width > kDataWidth)
    throw std::domain_error("NBitVector::NBitVector - Bit width is too wide.");

  data_.resize(numElements * bit_width / kDataWidth + 1);
}

NBitVector::NBitVector(size_t bit_width) noexcept
    : bit_width_{bit_width}, all_one_bit_width_{~(kAllOnes << bit_width)} {}

NBitVector::~NBitVector() noexcept = default;

NBitVector::NBitVector(const NBitVector&) = default;

NBitVector::NBitVector(NBitVector&&) noexcept = default;

NBitVector& NBitVector::operator=(const NBitVector&) = default;

NBitVector& NBitVector::operator=(NBitVector&&) noexcept = default;

uint64_t NBitVector::Get(size_t idx) const {
  if (idx >= num_elements_)
    throw std::out_of_range("NBitVector::Get - Index out of range");
  return GetUnsafe(idx);
}

uint64_t NBitVector::GetUnsafe(size_t idx) const noexcept {
  size_t total_bit_offset = bit_width_ * idx;

  size_t data_idx = total_bit_offset >> kDataWidthLog2;
  size_t bit_pos = total_bit_offset & kDataBitRemainderMask;
  const uint64_t* p = data_.data() + data_idx;

#ifdef __SIZEOF_INT128__
  unsigned __int128 chunk = *reinterpret_cast<const unsigned __int128*>(p);
  return (chunk >> bit_pos) & all_one_bit_width_;
#else
  uint64_t result = p[0] >> bit_pos;
  const uint64_t overflow_mask = overflow_table_[bit_width_][bit_pos];
  if (overflow_mask != 0) [[unlikely]] {
    const uint64_t high_bits = data_[data_idx + 1];
    result |= high_bits << (kDataWidth - bit_pos);
  }
  return result & all_one_bit_width_;
#endif
}

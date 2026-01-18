#include "Utils/Containers/NBitVector.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "Utils/Assert.h"

namespace {
bool IsPowerOf2AndSmall(size_t bit_width) {
  return bit_width <= 32 && (bit_width & (bit_width - 1)) == 0;
}
}  // namespace

NBitVector::NBitVector() noexcept
    : bit_width_{1}, all_one_bit_width_{1}, is_power_of_2_{true} {}

NBitVector::NBitVector(size_t numElements, size_t bit_width)
    : bit_width_{bit_width},
      all_one_bit_width_{~(kAllOnes << bit_width)},
      is_power_of_2_{IsPowerOf2AndSmall(bit_width)},
      num_elements_{numElements} {
  GAME_ASSERT(bit_width <= kDataWidth, "Bit width is too wide.");
  data_.resize(numElements * bit_width / kDataWidth + 1);
}

NBitVector::NBitVector(size_t bit_width) noexcept
    : bit_width_{bit_width},
      all_one_bit_width_{~(kAllOnes << bit_width)},
      is_power_of_2_{IsPowerOf2AndSmall(bit_width)} {}

NBitVector::~NBitVector() noexcept = default;

NBitVector::NBitVector(const NBitVector&) = default;

NBitVector::NBitVector(NBitVector&&) noexcept = default;

NBitVector& NBitVector::operator=(const NBitVector&) = default;

NBitVector& NBitVector::operator=(NBitVector&&) noexcept = default;

uint64_t NBitVector::Get(size_t idx) const noexcept {
  GAME_ASSERT(idx < num_elements_, "Index out of range");
  const size_t bit_width = bit_width_;  // Cache member as local
  const size_t total_bit_offset = bit_width * idx;

  const size_t data_idx = total_bit_offset >> kDataWidthLog2;
  const size_t bit_pos = total_bit_offset & kDataBitRemainderMask;
  const uint64_t* GAME_RESTRICT p = data_.data() + data_idx;

#ifdef __SIZEOF_INT128__
  unsigned __int128 chunk;
  std::memcpy(&chunk, p, sizeof(chunk));
  return (chunk >> bit_pos) & all_one_bit_width_;
#else
  uint64_t result = p[0] >> bit_pos;
  // Power-of-2 bit widths <= 32 never cross word boundaries
  if (!is_power_of_2_) [[unlikely]] {
    const uint64_t overflow_mask = ComputeOverflowMask(bit_pos);
    if (overflow_mask != 0) {
      const uint64_t high_bits = p[1];
      result |= high_bits << (kDataWidth - bit_pos);
    }
  }
  return result & all_one_bit_width_;
#endif
}

void NBitVector::Reserve(size_t num_elements) {
  size_t required_words =
      (num_elements * bit_width_ + kDataWidth - 1) / kDataWidth +
      1;  // +1 for padding
  data_.reserve(required_words);
}

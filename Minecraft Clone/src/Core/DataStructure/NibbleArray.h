#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>

template <size_t NumElements>
class NibbleArray {
 public:
  NibbleArray();
  ~NibbleArray();
  uint8_t Get(size_t index) const noexcept;
  void Set(size_t index, uint8_t new_val) noexcept;
  void Fill(uint8_t new_val);

  /*
  Not concurrency safe
  */
  bool operator==(const NibbleArray<NumElements>&) const noexcept;

 private:
  std::array<uint8_t, (NumElements + 1) / 2> data;
};

template <size_t NumElements>
NibbleArray<NumElements>::NibbleArray() = default;

template <size_t NumElements>
NibbleArray<NumElements>::~NibbleArray() = default;

template <size_t NumElements>
uint8_t NibbleArray<NumElements>::Get(size_t index) const noexcept {
  size_t byte_idx = index >> 1;
  int shift = (index & 1) * 4;
  return (data[byte_idx] >> shift) & 0xF;
}

template <size_t NumElements>
void NibbleArray<NumElements>::Set(size_t index, uint8_t new_val) noexcept {
  assert(new_val < 16);
  size_t byte_idx = index >> 1;
  int shift = (index & 1) * 4;
  uint8_t nibble_mask = uint8_t(0xFu << shift);
  data[byte_idx] &= ~nibble_mask;
  data[byte_idx] |= new_val << shift;
}

template <size_t NumElements>
void NibbleArray<NumElements>::Fill(uint8_t new_val) {
  assert(new_val < 16);
  data.fill(new_val | (new_val << 4));
}

template <size_t NumElements>
bool NibbleArray<NumElements>::operator==(
    const NibbleArray<NumElements>& rhs) const noexcept {
  constexpr size_t kBytes = sizeof(data);
  if constexpr (kBytes == 8) {
    uint64_t a, b;
    std::memcpy(&a, data.data(), 8);
    std::memcpy(&b, rhs.data.data(), 8);
    return a == b;
  } else if constexpr (kBytes < 8) {
    // Small sizes: a single 64-bit load is still fine, but we must mask
    // off the bytes that do not belong to the array.
    uint64_t a = 0, b = 0;
    std::memcpy(&a, data.data(), kBytes);
    std::memcpy(&b, rhs.data.data(), kBytes);
    uint64_t mask = (1ULL << (8 * kBytes)) - 1ULL;
    return (a & mask) == (b & mask);
  } else {
    // Generic path: memcmp is vectorised and as fast as manual loops.
    return std::memcmp(data.data(), rhs.data.data(), kBytes) == 0;
  }
}
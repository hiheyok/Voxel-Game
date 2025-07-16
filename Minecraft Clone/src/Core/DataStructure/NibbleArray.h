#include <array>
#include <cassert>
#include <cstdint>

template <size_t NumElements>
class NibbleArray {
 public:
  NibbleArray();
  ~NibbleArray();
  uint8_t Get(size_t index) const;
  void Set(size_t index, uint8_t new_val);
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
uint8_t NibbleArray<NumElements>::Get(size_t index) const {
  size_t byte_idx = index >> 1;
  int shift = (index & 1) * 4;
  return (data[byte_idx] >> shift) & 0xF;
}

template <size_t NumElements>
void NibbleArray<NumElements>::Set(size_t index, uint8_t new_val) {
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
  // Compare using 64 bit for speedup

  const uint64_t* lhs_ptr = reinterpret_cast<const uint64_t*>(data.data());
  const uint64_t* rhs_ptr = reinterpret_cast<const uint64_t*>(rhs.data.data());

  static constexpr size_t kLen = NumElements / (2 * 8);
  static constexpr size_t kPadding = (NumElements / 2) - kLen * 8;

  for (size_t i = 0; i < kLen; ++i) {
    if (*(rhs_ptr++) != *(lhs_ptr++)) return false;
  }

  for (size_t i = kLen * 8; i < kLen * 8 + kPadding; ++i) {
    if (data[i] != rhs.data[i]) return false;
  }

  return true;
}
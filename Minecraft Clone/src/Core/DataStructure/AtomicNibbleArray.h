#include <array>
#include <atomic>
#include <cassert>
#include <cstdint>

template <size_t NumElements>
class AtomicNibbleArray {
 public:
  AtomicNibbleArray();
  ~AtomicNibbleArray();
  uint8_t Get(size_t index) const;
  void Set(size_t index, uint8_t new_val);
  void Fill(uint8_t new_val);

  /*
  Not concurrency safe
  */
  bool operator==(const AtomicNibbleArray<NumElements>&) const noexcept;

 private:
  std::array<uint8_t, (NumElements + 1) / 2> data;
};

template <size_t NumElements>
AtomicNibbleArray<NumElements>::AtomicNibbleArray() = default;

template <size_t NumElements>
AtomicNibbleArray<NumElements>::~AtomicNibbleArray() = default;

template <size_t NumElements>
uint8_t AtomicNibbleArray<NumElements>::Get(size_t index) const {
  size_t byte_idx = index >> 1;
  int shift = (index & 1) * 4;
  uint8_t b = std::atomic_ref(data[byte_idx]).load(std::memory_order_relaxed);
  return (b >> shift) & 0xF;
}

template <size_t NumElements>
void AtomicNibbleArray<NumElements>::Set(size_t index, uint8_t new_val) {
  assert(new_val < 16);
  size_t byte_idx = index >> 1;
  int shift = (index & 1) * 4;
  uint8_t nibble_mask = uint8_t(0xFu << shift);

  auto ref = std::atomic_ref<uint8_t>(data[byte_idx]);
  uint8_t old_b = ref.load(std::memory_order_relaxed);
  uint8_t new_b;
  do {
    new_b =
        uint8_t((old_b & ~nibble_mask) | ((new_val << shift) & nibble_mask));
  } while (!ref.compare_exchange_weak(old_b, new_b, std::memory_order_release,
                                      std::memory_order_relaxed));
}

template <size_t NumElements>
void AtomicNibbleArray<NumElements>::Fill(uint8_t new_val) {
  assert(new_val < 16);

  uint8_t full_byte_value = (new_val << 4) | new_val;

  for (size_t byte_idx = 0; byte_idx < data.size(); ++byte_idx) {
    auto ref = std::atomic_ref<uint8_t>(data[byte_idx]);
    uint8_t old_b = ref.load(std::memory_order_relaxed);
    uint8_t new_b = full_byte_value;
    while (!ref.compare_exchange_weak(old_b, new_b, std::memory_order_release,
                                      std::memory_order_relaxed)) {
    };
  }
}

template <size_t NumElements>
bool AtomicNibbleArray<NumElements>::operator==(
    const AtomicNibbleArray<NumElements>& rhs) const noexcept {
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
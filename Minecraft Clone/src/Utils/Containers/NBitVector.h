#pragma once
#include <vector>
#include <array>
#include <stdexcept>

template <typename StorageBit = unsigned long long>
class NBitVector {
public:
    NBitVector(int numElements, int bitWidth);
    NBitVector(int bitWidth);
    NBitVector();
    NBitVector(const NBitVector&);
    NBitVector(NBitVector&&);
    NBitVector& operator=(const NBitVector&);
    NBitVector& operator=(NBitVector&&);
    ~NBitVector();

    StorageBit Get(size_t idx) const;
    StorageBit GetUnsafe(size_t idx) const;
    template <typename T>
    void Append(T val);

    template <typename T>
    void Set(size_t idx, T val);
    template <typename T>
    void SetUnsafe(size_t idx, T val) noexcept;
private:
    void ComputeMaskCache();

    std::vector<StorageBit> data_;
    int bit_width_ = 0;
    int num_elements_ = 0;

    static constexpr size_t kStorageBits = sizeof(StorageBit) * 8;
    static constexpr StorageBit kAllOnes = ~(static_cast<StorageBit>(0));
    StorageBit all_ones_bit_width_;

    std::array<StorageBit, kStorageBits> overflow_table_;
};

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector() {
    ComputeMaskCache();
}

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector(int numElements, int bitWidth) : bit_width_{ bitWidth }, num_elements_{ numElements } {
    if (bitWidth > kStorageBits)
        g_logger.LogError("NBitVector<StorageBit>::NBitVector", "Bit width is too wide.");

    data_.resize(numElements * bitWidth / kStorageBits + 1);
    all_ones_bit_width_ = ~(kAllOnes << bit_width_);
    ComputeMaskCache();
}

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector(int bitWidth) : bit_width_{ bitWidth } {
    all_ones_bit_width_ = ~(kAllOnes << bit_width_);
    ComputeMaskCache();
}

template<typename StorageBit>
NBitVector<StorageBit>::~NBitVector() {
    data_.clear();
}

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector(const NBitVector&) = default;

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector(NBitVector&&) = default;

template<typename StorageBit>
NBitVector<StorageBit>& NBitVector<StorageBit>::operator=(const NBitVector<StorageBit>&) = default;

template<typename StorageBit>
NBitVector<StorageBit>& NBitVector<StorageBit>::operator=(NBitVector<StorageBit>&&) = default;

template<typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::Set(size_t idx, T val) {
    if (static_cast<size_t>(val) >= (1ULL << bit_width_))
        g_logger.LogError("NBitVector<StorageBit>::Set", "Invalid number. Wrong size");
    if (idx >= num_elements_)
        g_logger.LogError("NBitVector<StorageBit>::Set", "Index out of range");
    SetUnsafe(idx, val);
}

template<typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::SetUnsafe(size_t idx, T val) noexcept {
    StorageBit data = static_cast<StorageBit>(val);

    size_t vectorIndex = bit_width_ * idx / kStorageBits;
    size_t integerIndex = (bit_width_ * idx) % kStorageBits;

    const StorageBit mask = all_ones_bit_width_ << integerIndex;
    const StorageBit& overflow_mask = overflow_table_[integerIndex];

    // Clear data first
    data_[vectorIndex] &= ~mask;
    data_[vectorIndex] |= data << integerIndex;

    // Insert the left over if it overlaps
    // If there is no overflow, overflow is 0 so it will do nothing to the data
    data_[vectorIndex + 1] &= ~overflow_mask;
    data_[vectorIndex + 1] |= overflow_mask & (data >> (kStorageBits - integerIndex));
}

template<typename StorageBit>
StorageBit NBitVector<StorageBit>::Get(size_t idx) const {
    if (idx >= num_elements_)
        g_logger.LogError("NBitVector<StorageBit>::Get", "Index out of range");
    return GetUnsafe(idx);
}

template<typename StorageBit>
StorageBit NBitVector<StorageBit>::GetUnsafe(size_t idx) const {
    size_t vectorIndex = bit_width_ * idx / kStorageBits;
    size_t integerIndex = (bit_width_ * idx) % kStorageBits;

    const StorageBit mask = all_ones_bit_width_ << integerIndex;
    const StorageBit& overflow_mask = overflow_table_[integerIndex];

    StorageBit data = (data_[vectorIndex] & mask) >> integerIndex;

    // If it overlap with the next bit extract the next bit too
    // If the overflow mask is 0, it means that there is no overflow
    // Because it is 0, it will set dataNext to 0 too and this will do nothing to the data
    StorageBit dataNext = data_[vectorIndex + 1] & overflow_mask;
    dataNext <<= (kStorageBits - integerIndex);
    data |= dataNext;

    return data;
}

template<typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::Append(T val) {
    if (val >= (1 << bit_width_))
        g_logger.LogError("NBitVector<StorageBit>::Append", "Invalid number. Wrong size");

    num_elements_++;
    if (num_elements_ * bit_width_ / kStorageBits >= data_.size() - 1) { // -1 to keep an extra space of padding
        data_.push_back(static_cast<StorageBit>(0));
    }

    Set(num_elements_ - 1, val);
}

template<typename StorageBit>
void NBitVector<StorageBit>::ComputeMaskCache() {
    for (size_t i = 0; i < kStorageBits; ++i) {
        overflow_table_[i] = (kStorageBits - i < bit_width_)
            ? (all_ones_bit_width_ >> (kStorageBits - i))
            : 0;
    }

}
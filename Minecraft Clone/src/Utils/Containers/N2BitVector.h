#pragma once
#include <vector>

#include "Utils/LogUtils.h"
// For sizes of base 2
template <typename StorageBit = unsigned long long>
class N2BitVector {
   private:
    std::vector<StorageBit> data_;
    int bit_width_ = 0;
    int num_elements_ = 0;

    static constexpr size_t storage_bits_ = sizeof(StorageBit) * 8;
    static constexpr StorageBit all_ones_ = ~(static_cast<StorageBit>(0));
    StorageBit all_ones_bit_width_;

    StorageBit GetMask(int idx) const;

   public:
    N2BitVector(int numElements, int bitWidth);
    N2BitVector(int bitWidth);
    ~N2BitVector();

    StorageBit Get(size_t idx) const;
    StorageBit GetUnsafe(size_t idx) const;
    template <typename T>
    void Append(T val);

    template <typename T>
    void Set(size_t idx, T val);
};

template <typename StorageBit>
template <typename T>
void N2BitVector<StorageBit>::Set(size_t idx, T val) {
    if (static_cast<size_t>(val) >= (1ULL << bit_width_))
        g_logger.LogError("N2BitVector<StorageBit>::Set",
                          "Invalid number. Wrong size");
    if (idx >= num_elements_)
        g_logger.LogError("N2BitVector<StorageBit>::Set", "Index out of range");
    StorageBit data = static_cast<StorageBit>(val);

    size_t vectorIndex = bit_width_ * idx / storage_bits_;
    size_t integerIndex = (bit_width_ * idx) & (storage_bits_ - 1);

    StorageBit mask = GetMask(integerIndex);

    // Clear data first
    data_[vectorIndex] &= ~mask;
    data_[vectorIndex] |= data << integerIndex;
}

template <typename StorageBit>
StorageBit N2BitVector<StorageBit>::Get(size_t idx) const {
    if (idx >= num_elements_)
        g_logger.LogError("N2BitVector<StorageBit>::Get", "Index out of range");
    return GetUnsafe(idx);
}

template <typename StorageBit>
StorageBit N2BitVector<StorageBit>::GetUnsafe(size_t idx) const {
    size_t vectorIndex = bit_width_ * idx / storage_bits_;
    size_t integerIndex = (bit_width_ * idx) & (storage_bits_ - 1);

    StorageBit mask = GetMask(integerIndex);

    return (data_[vectorIndex] & mask) >> integerIndex;
}

template <typename StorageBit>
N2BitVector<StorageBit>::N2BitVector(int numElements, int bitWidth)
    : bit_width_{bitWidth}, num_elements_{numElements} {
    if (bitWidth > storage_bits_)
        g_logger.LogError("N2BitVector<StorageBit>::NBitVector",
                          "Bit width is too wide.");
    if ((bitWidth & (bitWidth - 1)) != 0)
        g_logger.LogError("N2BitVector<StorageBit>::NBitVector",
                          "Bit width is not base 2");
    data_.resize(numElements * bitWidth / storage_bits_ + 1);
    all_ones_bit_width_ = ~(all_ones_ << bit_width_);
}

template <typename StorageBit>
N2BitVector<StorageBit>::N2BitVector(int bitWidth) : bit_width_{bitWidth} {
    all_ones_bit_width_ = ~(all_ones_ << bit_width_);
}

template <typename StorageBit>
N2BitVector<StorageBit>::~N2BitVector() {
    data_.clear();
}

template <typename StorageBit>
template <typename T>
void N2BitVector<StorageBit>::Append(T val) {
    if (val >= (1 << bit_width_))
        g_logger.LogError("N2BitVector<StorageBit>::Append",
                          "Invalid number. Wrong size");
    num_elements_++;
    if (num_elements_ * bit_width_ / storage_bits_ >= data_.size()) {
        data_.push_back(static_cast<StorageBit>(0));
    }

    Set(num_elements_ - 1, val);
}

template <typename StorageBit>
StorageBit N2BitVector<StorageBit>::GetMask(int integerIndex) const {
    return all_ones_bit_width_ << integerIndex;
}
#pragma once
#include <vector>
#include <stdexcept>
#include <exception>
// TODO: Add precomputed mask for all instances
template <typename StorageBit = unsigned long long>
class NBitVector {
private:
    std::vector<StorageBit> data_;
    int bit_width_ = 0;
    int num_elements_ = 0;

    static constexpr const size_t storage_bits_ = sizeof(StorageBit) * 8;
    static constexpr const StorageBit all_ones_ = ~(static_cast<StorageBit>(0));
    StorageBit all_ones_bit_width_;

    std::pair<StorageBit, StorageBit> GetMask(size_t idx) const;

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
};

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector() = default;

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
    if (static_cast<size_t>(val) >= (1ULL << bit_width_)) throw std::runtime_error("Invalid number. Wrong size");
    if (idx >= num_elements_) throw std::out_of_range("Index out of range");

    StorageBit data = static_cast<StorageBit>(val);

    size_t vectorIndex = bit_width_ * idx / storage_bits_;
    size_t integerIndex = (bit_width_ * idx) % storage_bits_;

    const auto [mask, overflowMask] = GetMask(integerIndex);

    // Clear data first
    data_[vectorIndex] &= ~mask;
    data_[vectorIndex] |= data << integerIndex;

    // Insert the left over if it overlaps

    if (overflowMask != 0) {
        data_[vectorIndex + 1] &= ~overflowMask;
        data_[vectorIndex + 1] |= data >> (storage_bits_ - integerIndex);
    }
}

template<typename StorageBit>
StorageBit NBitVector<StorageBit>::Get(size_t idx) const {
    if (idx >= num_elements_) throw std::out_of_range("Index out of range");
    return GetUnsafe(idx);
}

template<typename StorageBit>
StorageBit NBitVector<StorageBit>::GetUnsafe(size_t idx) const {
    size_t vectorIndex = bit_width_ * idx / storage_bits_;
    size_t integerIndex = (bit_width_ * idx) % storage_bits_;

    const auto [mask, overflowMask] = GetMask(integerIndex);

    StorageBit data = (data_[vectorIndex] & mask) >> integerIndex;;

    // If it overlap with the next bit extract the next bit too
    if (overflowMask != 0) {
        StorageBit dataNext = data_[vectorIndex + 1] & overflowMask;
        dataNext <<= (storage_bits_ - integerIndex);
        data |= dataNext;
    }

    return data;
}

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector(int numElements, int bitWidth) : bit_width_{ bitWidth }, num_elements_{ numElements } {
    if (bitWidth > storage_bits_) throw std::runtime_error("Bit width is too wide.");
    data_.resize(numElements * bitWidth / storage_bits_ + 1);
    all_ones_bit_width_ = ~(all_ones_ << bit_width_);
}

template<typename StorageBit>
NBitVector<StorageBit>::NBitVector(int bitWidth) : bit_width_{ bitWidth } {
    all_ones_bit_width_ = ~(all_ones_ << bit_width_);
}

template<typename StorageBit>
NBitVector<StorageBit>::~NBitVector() {
    data_.clear();
}

template<typename StorageBit>
template <typename T>
void NBitVector<StorageBit>::Append(T val) {
    if (val >= (1 << bit_width_)) throw std::runtime_error("Invalid number. Wrong size");
    num_elements_++;
    if (num_elements_ * bit_width_ / storage_bits_ >= data_.size()) {
        data_.push_back(static_cast<StorageBit>(0));
    }

    Set(num_elements_ - 1, val);
}

template<typename StorageBit>
std::pair<StorageBit, StorageBit> NBitVector<StorageBit>::GetMask(size_t integerIndex) const {
    StorageBit mask = all_ones_bit_width_ << integerIndex;
    StorageBit overflowMask = 0;

    overflowMask |= (storage_bits_ - integerIndex < bit_width_) * (all_ones_bit_width_ >> (storage_bits_ - integerIndex));

    return { mask, overflowMask };
}
#ifndef SRC_UTILS_ITERATORS_H_
#define SRC_UTILS_ITERATORS_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>

#include "Utils/Assert.h"

template <size_t N, typename T>
class Product;

template <size_t N, typename T>
class ProductIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = std::array<T, N>;
  using pointer = const value_type*;
  using reference = const value_type;

  ProductIterator() noexcept = default;

  reference operator*() const noexcept { return val_; }
  pointer operator->() const noexcept { return &val_; }

  ProductIterator& operator++() noexcept {
    ++idx_;

    // Fast path
    if (++val_[N - 1] < bounds_[N - 1].second) [[likely]] {
      return *this;
    }

    val_[N - 1] = bounds_[N - 1].first;
    for (size_t i = N - 1; i-- > 0;) {
      if (++val_[i] < bounds_[i].second) [[likely]] {
        return *this;
      }
      val_[i] = bounds_[i].first;
    }
    return *this;
  }

  ProductIterator operator++(int) noexcept {
    ProductIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  ProductIterator& operator--() noexcept {
    if (idx_ == max_) [[unlikely]] {
      --idx_;
      for (size_t i = 0; i < N; ++i) {
        val_[i] = bounds_[i].second - 1;
      }
      return *this;
    }

    --idx_;

    // Fast path
    if (--val_[N - 1] >= bounds_[N - 1].first) [[likely]] {
      return *this;
    }

    val_[N - 1] = bounds_[N - 1].second - 1;
    for (size_t i = N - 1; i-- > 0;) {
      if (--val_[i] >= bounds_[i].first) [[likely]] {
        return *this;
      }
      val_[i] = bounds_[i].second - 1;
    }
    return *this;
  }

  ProductIterator operator--(int) noexcept {
    ProductIterator tmp = *this;
    --(*this);
    return tmp;
  }

  friend bool operator==(const ProductIterator<N, T>& a,
                         const ProductIterator<N, T>& b) noexcept = default;

 private:
  friend Product<N, T>;

  explicit ProductIterator(size_t idx, size_t max,
                           const std::pair<T, T>* bounds) noexcept
      : idx_{idx}, max_{max}, bounds_{bounds} {
    if (idx == 0) {
      for (size_t i = 0; i < N; ++i) {
        val_[i] = bounds[i].first;
      }
    } else {
      for (size_t i = N; i-- > 0;) {
        size_t size = bounds[i].second - bounds[i].first;
        val_[i] = bounds[i].first + static_cast<T>(idx % size);
        idx /= size;
      }
    }
  }

  size_t idx_{};
  size_t max_{};
  const std::pair<T, T>* bounds_{};
  std::array<T, N> val_{};
};

template <size_t N, typename T = int>
class Product {
  static_assert(N != 0);

 public:
  using Range = std::pair<T, T>;

  template <typename... Pairs>
  explicit Product(Pairs... pairs) noexcept {
    static_assert(sizeof...(pairs) == N,
                  "Incorrect number of ranges provided.");

    std::array<Range, N> bounds = {pairs...};
    Init(bounds);
  }

  explicit Product(std::initializer_list<std::pair<T, T>> bounds) noexcept {
    GAME_ASSERT(bounds.size() == N, "Incorrect number of ranges provided");
    std::array<Range, N> arr;
    std::copy_n(bounds.begin(), N, arr.data());
    Init(arr);
  }

  explicit Product(std::initializer_list<T> bounds) noexcept {
    GAME_ASSERT(bounds.size() == N, "Incorrect number of ranges provided");
    std::array<Range, N> arr;
    auto it = bounds.begin();
    for (size_t i = 0; i < N; ++i, ++it) {
      arr[i] = {T{}, *it};
    }
    Init(arr);
  }

  explicit Product(T end) noexcept {
    std::array<Range, N> arr;
    for (size_t i = 0; i < N; ++i) {
      arr[i] = {T{}, end};
    }
    Init(arr);
  }

  explicit Product(T beg, T end) noexcept {
    std::array<Range, N> arr;
    for (size_t i = 0; i < N; ++i) {
      arr[i] = {beg, end};
    }
    Init(arr);
  }

  ProductIterator<N, T> begin() const noexcept {
    return ProductIterator<N, T>(0, max_, bounds_.data());
  }
  ProductIterator<N, T> end() const noexcept {
    return ProductIterator<N, T>(max_, max_, bounds_.data());
  }

 private:
  void Init(const std::array<Range, N>& bounds) noexcept {
    bounds_ = bounds;
    max_ = 1;
    for (size_t i = 0; i < N; ++i) {
      GAME_ASSERT(bounds[i].second > bounds[i].first, "Invalid range bounds");
      size_t size = bounds[i].second - bounds[i].first;
      // size > 0 is guaranteed by the assert above
      if (max_ > std::numeric_limits<size_t>::max() / size) {
        GAME_ASSERT(false, "Overflow detected in calculating product size");
        max_ = 0;
        return;
      }
      max_ *= size;
    }
  }

  std::array<Range, N> bounds_;
  size_t max_;
};

// Assertion just to verify
static_assert(std::bidirectional_iterator<ProductIterator<2, int>>);

#endif  // SRC_UTILS_ITERATORS_H_

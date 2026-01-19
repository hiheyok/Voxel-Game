#pragma once

#include <algorithm>
#include <array>
#include <type_traits>
#include <vector>

// A priority queue implementation using buckets for O(1) enqueue/dequeue
// when priority levels are bounded integers. Also known as Dial's algorithm.
//
// Template parameters:
//   T - The element type to store
//   NumBuckets - Number of priority levels (e.g., 16 for light levels 0-15)
//   GetPriority - Functor or lambda returning priority level (0 to NumBuckets-1)
//
// Items are dequeued from highest priority bucket first.
template <typename T, size_t NumBuckets, typename GetPriority>
class BucketQueue {
 public:
  // Determine level type from GetPriority return type (unsigned for array indexing)
  using LevelType = std::invoke_result_t<GetPriority, const T&>;

  explicit BucketQueue(GetPriority get_priority = GetPriority{})
      : get_priority_{get_priority},
        current_level_{static_cast<LevelType>(NumBuckets - 1)} {}

  // Add an item to the appropriate bucket based on its priority
  void Push(const T& item) {
    LevelType level = get_priority_(item);
    current_level_ = std::max(current_level_, level);
    buckets_[level].push_back(item);
    ++size_;
  }

  void Push(T&& item) {
    LevelType level = get_priority_(item);
    current_level_ = std::max(current_level_, level);
    buckets_[level].push_back(std::move(item));
    ++size_;
  }

  // Try to pop the highest priority item
  // Returns true if an item was popped, false if queue is empty
  bool TryPop(T& item) noexcept {
    // Fast path: check if empty using cached size
    if (size_ == 0) [[unlikely]] {
      return false;
    }

    // Safe unsigned decrement: check before decrement
    for (LevelType level = current_level_ + 1; level-- > 0;) {
      auto& bucket = buckets_[level];
      if (!bucket.empty()) {
        item = std::move(bucket.back());
        bucket.pop_back();
        --size_;
        current_level_ = level;
        return true;
      }
    }
    return false;
  }

  // Check if the queue is empty - O(1)
  [[nodiscard]] bool Empty() const noexcept { return size_ == 0; }

  // Clear all buckets and reset state
  void Clear() noexcept {
    for (auto& bucket : buckets_) {
      bucket.clear();
    }
    current_level_ = static_cast<LevelType>(NumBuckets - 1);
    size_ = 0;
  }

  // Reserve capacity in all buckets
  void Reserve(size_t capacity_per_bucket) {
    for (auto& bucket : buckets_) {
      bucket.reserve(capacity_per_bucket);
    }
  }

  // Apply a function to each element in the queue (for delayed processing)
  // Iterates in priority order (highest first)
  template <typename Func>
  void ForEach(Func&& func) {
    for (LevelType level = current_level_ + 1; level-- > 0;) {
      for (auto& item : buckets_[level]) {
        func(item);
      }
    }
  }

  template <typename Func>
  void ForEach(Func&& func) const {
    for (LevelType level = current_level_ + 1; level-- > 0;) {
      for (const auto& item : buckets_[level]) {
        func(item);
      }
    }
  }

  // Pop the entire highest-priority non-empty bucket for direct iteration
  // Returns empty vector if queue is empty. O(1) swap operation.
  std::vector<T> PopBucket() noexcept {
    std::vector<T> result;
    if (size_ == 0) {
      return result;
    }
    
    for (LevelType level = current_level_ + 1; level-- > 0;) {
      if (!buckets_[level].empty()) {
        std::swap(result, buckets_[level]);
        size_ -= result.size();
        current_level_ = (level > 0) ? level - 1 : 0;
        return result;
      }
    }
    return result;
  }

  // Get total size across all buckets - O(1)
  [[nodiscard]] size_t Size() const noexcept { return size_; }

 private:
  std::array<std::vector<T>, NumBuckets> buckets_;
  GetPriority get_priority_;
  LevelType current_level_;
  size_t size_ = 0;
};

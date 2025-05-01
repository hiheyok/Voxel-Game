#ifndef UTILS_CONTAINERS_FIFO_QUEUE_H
#define UTILS_CONTAINERS_FIFO_QUEUE_H

#include <vector>
template <class T, size_t kQueueSize>
class FixedFIFOQueue {
 public:
  static_assert(kQueueSize > 0, "kQueueSize must be greater than 0");

  FixedFIFOQueue();
  ~FixedFIFOQueue();

  FixedFIFOQueue(FixedFIFOQueue&& q) = delete;
  FixedFIFOQueue(const FixedFIFOQueue& q) = delete;

  FixedFIFOQueue& operator=(FixedFIFOQueue&&) = delete;
  FixedFIFOQueue& operator=(const FixedFIFOQueue&) = delete;

  void push(const T& obj) noexcept;
  T get() noexcept;

  void ResetData() noexcept;
  bool IsEmpty() const noexcept;
  bool IsFull() const noexcept;

 private:
  std::array<T, kQueueSize> buffer_;

  size_t start_ptr_ = 0;
  size_t end_ptr_ = 0;
  size_t element_count_ = 0;
};

template <class T, size_t kQueueSize>
FixedFIFOQueue<T, kQueueSize>::FixedFIFOQueue() : buffer_{} {}

template <class T, size_t kQueueSize>
FixedFIFOQueue<T, kQueueSize>::~FixedFIFOQueue() = default;

template <class T, size_t kQueueSize>
void FixedFIFOQueue<T, kQueueSize>::push(const T& obj) noexcept {
  assert(!IsFull() && "Attempt to push to a full queue");
  buffer_[end_ptr_++] = obj;
  end_ptr_ = end_ptr_ % kQueueSize;
  ++element_count_;
}

template <class T, size_t kQueueSize>
T FixedFIFOQueue<T, kQueueSize>::get() noexcept {
  assert(!IsEmpty() && "Attempt to get from an empty queue");
  T out = buffer_[start_ptr_++];
  start_ptr_ = start_ptr_ % kQueueSize;
  --element_count_;
  return out;
}

template <class T, size_t kQueueSize>
void FixedFIFOQueue<T, kQueueSize>::ResetData() noexcept {
  start_ptr_ = 0;
  end_ptr_ = 0;
  element_count_ = 0;
}

template <class T, size_t kQueueSize>
bool FixedFIFOQueue<T, kQueueSize>::IsEmpty() const noexcept {
  return element_count_ <= 0;
}

template <class T, size_t kQueueSize>
bool FixedFIFOQueue<T, kQueueSize>::IsFull() const noexcept {
  return element_count_ >= kQueueSize;
}

#endif

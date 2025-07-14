// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <mutex>
#include <queue>
#include <utility>

template <class T>
class ConcurrentQueue {
 public:
  bool Empty() {
    std::unique_lock<std::mutex> lock{mutex_};
    return queue_.empty();
  }

  void PopAll(std::vector<T>& out) {
    std::unique_lock<std::mutex> lock{mutex_};
    out.reserve(out.size() + queue_.size());
    while (!queue_.empty()) {
      out.push_back(std::move(queue_.front()));
      queue_.pop();
    }
  }

  T Pop() {
    std::unique_lock<std::mutex> lock{mutex_};
    T out = std::move(queue_.front());
    queue_.pop();
    return out;
  }

  void Push(T item) {
    std::unique_lock<std::mutex> lock{mutex_};
    queue_.push(std::move(item));
  }

 private:
  std::mutex mutex_;
  std::queue<T> queue_;
};

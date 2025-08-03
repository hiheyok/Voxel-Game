#pragma once

template <typename T>
class AssetHandle {
 public:
  AssetHandle() : p_{nullptr} {}
  explicit AssetHandle(T* p) : p_{p} {}

  T* operator->() const { return p_; }
  T& operator*() const { return *p_; }
  T* Get() const { return p_; }

  explicit operator bool() const { return p_ != nullptr; }
 private:
  T* p_;
};

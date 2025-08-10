#pragma once

#include <concepts>
#include <type_traits>

template <typename T>
class AssetHandle {
 public:
  AssetHandle() : p_{nullptr} {}
  explicit AssetHandle(T* p) : p_{p} {}

  template <typename Derived>
    requires std::is_base_of_v<T, Derived>
  explicit AssetHandle(const AssetHandle<Derived>& other)
      : AssetHandle{static_cast<T*>(other.Get())} {}

  T* operator->() const { return p_; }
  T& operator*() const { return *p_; }
  T* Get() const { return p_; }

  explicit operator bool() const { return p_ != nullptr; }

 private:
  T* p_;
};

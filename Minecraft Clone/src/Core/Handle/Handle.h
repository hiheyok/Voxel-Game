#pragma once

#include <concepts>
#include <type_traits>

template <typename T>
class Handle {
 public:
  Handle() : p_{nullptr} {}
  explicit Handle(T* p) : p_{p} {}

  template <typename Derived>
    requires std::is_base_of_v<T, Derived>
  explicit Handle(const Handle<Derived>& other)
      : Handle{static_cast<T*>(other.Get())} {}

  T* operator->() const { return p_; }
  T& operator*() const { return *p_; }
  T* Get() const { return p_; }

  explicit operator bool() const { return p_ != nullptr; }
  bool HasValue() const noexcept { return p_ != nullptr; }

 private:
  T* p_;
};

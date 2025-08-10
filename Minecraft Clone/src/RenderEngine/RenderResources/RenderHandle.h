#pragma once

#include <concepts>
#include <type_traits>

template <typename T>
class RenderHandle {
 public:
  RenderHandle() : p_{nullptr} {}
  explicit RenderHandle(T* p) : p_{p} {}

  template <typename Derived>
    requires std::is_base_of_v<T, std::decay_t<Derived>>
  RenderHandle(const RenderHandle<Derived>& p)
      : RenderHandle{static_cast<T*>(p.Get())} {}

  T* operator->() const { return p_; }
  T& operator*() const { return *p_; }
  T* Get() const { return p_; }

  explicit operator bool() const { return p_ != nullptr; }

 private:
  T* p_;
};

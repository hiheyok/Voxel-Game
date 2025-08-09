#pragma once

template <typename RenderResourceType>
class RenderHandle {
 public:
  RenderHandle() : p_{nullptr} {}
  explicit RenderHandle(RenderResourceType* p) : p_{p} {}

  RenderResourceType* operator->() const { return p_; }
  RenderResourceType& operator*() const { return *p_; }
  RenderResourceType* Get() const { return p_; }

  explicit operator bool() const { return p_ != nullptr; }

 private:
  RenderResourceType* p_;
};

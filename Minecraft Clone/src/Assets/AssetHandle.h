#pragma once

template <typename AssetType>
class AssetHandle {
 public:
  AssetHandle() : p_{nullptr} {}
  explicit AssetHandle(AssetType* p) : p_{p} {}

  AssetType* operator->() const { return p_; }
  AssetType& operator*() const { return *p_; }
  AssetType* Get() const { return p_; }

  explicit operator bool() const { return p_ != nullptr; }
 private:
  AssetType* p_;
};

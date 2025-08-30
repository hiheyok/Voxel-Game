#pragma once

struct HandleControlBlock {
  void* ptr_ = nullptr;
  int version_ = 1;
  bool is_destroyed_ = false;
};

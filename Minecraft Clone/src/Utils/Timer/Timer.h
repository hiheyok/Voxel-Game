// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <chrono>
#include <string>

class Timer {
 public:
  Timer() noexcept;

  double GetTimePassed_ns() const noexcept;
  double GetTimePassed_us() const noexcept;
  double GetTimePassed_ms() const noexcept;
  double GetTimePassed_s() const noexcept;

  std::string StrGetTimePassed_ns() const;
  std::string StrGetTimePassed_us() const;
  std::string StrGetTimePassed_ms() const;
  std::string StrGetTimePassed_s() const;

  void Set();

 private:
  std::chrono::high_resolution_clock::time_point first_;
};

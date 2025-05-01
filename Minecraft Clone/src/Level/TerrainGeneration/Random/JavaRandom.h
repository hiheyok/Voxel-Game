#pragma once
#include <ctime>
#include <random>

class JavaRandom {
 public:
  JavaRandom() noexcept;

  JavaRandom(int64_t seed) noexcept;

  void SetSeed(int64_t seed) noexcept;

  int Next(int bits) noexcept;

  int NextInt(int n);

  int NextInt() noexcept;

  uint64_t NextLong() noexcept;

  bool NextBoolean() noexcept;

  float NextFloat() noexcept;

  double NextDouble() noexcept;

 private:
  bool have_next_next_gaussian_;
  double next_next_gaussian;
  int64_t rseed_;
  int64_t serial_version_uid_ = 3905348978240129619L;
};

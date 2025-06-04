#include "Level/TerrainGeneration/Random/JavaRandom.h"

#include <stdexcept>

#include "Utils/LogUtils.h"

JavaRandom::JavaRandom() noexcept : JavaRandom{static_cast<int64_t>(time(0))} {}

JavaRandom::JavaRandom(int64_t seed) noexcept { SetSeed(seed); }

void JavaRandom::SetSeed(int64_t seed) noexcept {
  rseed_ = (seed ^ 0x5DEECE66DLL) & ((1LL << 48) - 1);
  have_next_next_gaussian_ = false;
}

int JavaRandom::Next(int bits) noexcept {
  rseed_ = (rseed_ * 0x5DEECE66DLL + 0xBLL) & ((1LL << 48) - 1);
  return static_cast<int>(rseed_ >> (48 - bits));
}

int JavaRandom::NextInt(int n) {
  if (n <= 0) throw std::domain_error("JavaRandom::NextInt - n must be positive");
  if ((n & (-n)) == n)  // n is power of 2
    return static_cast<int>((n * static_cast<int64_t>(Next(31))) >> 31);
  int bits, val;
  do {
    bits = Next(31);
    val = bits % n;
  } while (bits - val + (n - 1) < 0);
  return val;
}

int JavaRandom::NextInt() noexcept { return Next(32); }

int64_t JavaRandom::NextLong() noexcept {
  return (static_cast<int64_t>(Next(32)) << 32) + Next(32);
}

bool JavaRandom::NextBoolean() noexcept { return Next(1) != 0; }

float JavaRandom::NextFloat() noexcept {
  return Next(24) / static_cast<float>(1 << 24);
}

double JavaRandom::NextDouble() noexcept {
  return ((static_cast<int64_t>(Next(26)) << 27) + Next(27)) /
         static_cast<double>(1LL << 53);
}

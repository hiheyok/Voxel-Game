// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <cmath>
#include <cstddef>
#include <ostream>
#include <string>
#include <type_traits>

template <typename Derived>
class Position {
 public:
  template <typename T>
  constexpr Position(T px, T py, T pz) noexcept
      : x{static_cast<int>(px)},
        y{static_cast<int>(py)},
        z{static_cast<int>(pz)} {}

  constexpr Position(int px, int py, int pz) noexcept : x{px}, y{py}, z{pz} {}

  constexpr Position() noexcept = default;

  constexpr Position(const Position<Derived>& m) noexcept = default;
  constexpr Position(Position<Derived>&&) noexcept = default;
  constexpr Position& operator=(const Position<Derived>&) noexcept = default;
  constexpr Position& operator=(Position<Derived>&&) noexcept = default;
  constexpr ~Position() noexcept = default;

  constexpr void Set(int px, int py, int pz) noexcept {
    x = px;
    y = py;
    z = pz;
  }

  Derived& IncrementSide(int side, int val) noexcept {
    const int direction = 1 - 2 * (side & 1);
    const int axis = side >> 1;

    v[axis] += direction * val;
    return *static_cast<Derived*>(this);
  }

  int& operator[](int axis) noexcept {
    return v[axis];
  }

  const int& operator[](int axis) const noexcept {
    return v[axis];
  }

  [[nodiscard]] constexpr Derived operator-(
      const Derived& other) const noexcept {
    return Derived{x - other.x, y - other.y, z - other.z};
  }

  [[nodiscard]] constexpr Derived operator+(
      const Derived& other) const noexcept {
    return Derived{x + other.x, y + other.y, z + other.z};
  }

  template <typename T>
  [[nodiscard]] constexpr Derived operator*(const T& other) const noexcept {
    return Derived{x * other, y * other, z * other};
  }

  template <typename T>  // Division will floor the nums
  [[nodiscard]] constexpr Derived operator/(const T& other) const noexcept {
    return Derived{floor(static_cast<double>(x) / other),
                   floor(static_cast<double>(y) / other),
                   floor(static_cast<double>(z) / other)};
  }

  template <typename T>
  [[nodiscard]] constexpr Derived operator<<(const T& other) const noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return Derived{x << other, y << other, z << other};
  }

  template <typename T>
  [[nodiscard]] constexpr Derived operator>>(const T& other) const noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return Derived{x >> other, y >> other, z >> other};
  }

  template <typename T>
  [[nodiscard]] constexpr Derived operator&(const T& other) const noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return Derived{x & other, y & other, z & other};
  }

  template <typename T>
  [[nodiscard]] constexpr Derived operator^(const T& other) const noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return Derived{x ^ other, y ^ other, z ^ other};
  }

  template <typename T>
  [[nodiscard]] constexpr Derived operator|(const T& other) const noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    return Derived{x | other, y | other, z | other};
  }

  template <typename T>
  constexpr Derived& operator<<=(const T& other) noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    x <<= other;
    y <<= other;
    z <<= other;
    return *this;
  }

  template <typename T>
  constexpr Derived& operator>>=(const T& other) noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    x >>= other;
    y >>= other;
    z >>= other;
    return *this;
  }

  template <typename T>
  constexpr Derived& operator&=(const T& other) noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    x &= other;
    y &= other;
    z &= other;
    return *this;
  }

  template <typename T>
  constexpr Derived& operator^=(const T& other) noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    x ^= other;
    y ^= other;
    z ^= other;
    return *this;
  }

  template <typename T>
  constexpr Derived& operator|=(const T& other) noexcept {
    static_assert(std::is_integral<T>::value, "Integral required.");
    x |= other;
    y |= other;
    z |= other;
    return *this;
  }

  template <typename T>
  constexpr Position<Derived>& operator*=(const T& other) noexcept {
    x *= other;
    y *= other;
    z *= other;
    return *this;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const Position<Derived>& m) {
    return os << '[' << m.x << ',' << m.y << ',' << m.z << ']';
  }

  friend std::string operator+(const char* str, const Position<Derived>& m) {
    std::string s = std::string(str) + "[" + std::to_string(m.x) + "," +
                    std::to_string(m.y) + "," + std::to_string(m.z) + "]";
    return s;
  }

  // for HashMaps and HashSets

  constexpr bool operator==(const Position<Derived>& other) const noexcept {
    return x == other.x && y == other.y && z == other.z;
  }

  constexpr bool operator!=(const Position<Derived>& other) const noexcept {
    return x != other.x || y != other.y || z != other.z;
  }

  constexpr Position<Derived>& operator-=(
      const Position<Derived>& other) noexcept {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  constexpr Position<Derived>& operator+=(
      const Position<Derived>& other) noexcept {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  constexpr Position<Derived>& operator*=(
      const Position<Derived>& other) noexcept {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
  }

  constexpr Position<Derived>& operator/=(
      const Position<Derived>& other) noexcept {
    x = static_cast<int>(floor(static_cast<double>(x) / other.x));
    y = static_cast<int>(floor(static_cast<double>(y) / other.y));
    z = static_cast<int>(floor(static_cast<double>(z) / other.z));
    return *this;
  }

  [[nodiscard]] constexpr size_t hash() const noexcept {
    size_t h = 2166136261u;  // FNV offset basis
    h ^= static_cast<size_t>(x);
    h *= 16777619u;  // FNV prime
    h ^= static_cast<size_t>(y);
    h *= 16777619u;
    h ^= static_cast<size_t>(z);
    h *= 16777619u;
    return h;
  }

  explicit operator std::string() const {
    return "[" + std::to_string(x) + "," + std::to_string(y) + "," +
           std::to_string(z) + "]";
  }

  union {
    struct {
      int x, y, z;
    };         // keeps .x .y .z syntax
    int v[3];  // allows operator[](i) and vector load
  };
};

// Define a concept to identify types that follow the CRTP pattern with Position
template <typename T>
concept PositionDerived = std::is_base_of<Position<T>, T>::value;

namespace std {
// Specialize std::hash<T> only for types T that satisfy PositionDerived
template <PositionDerived T>  // Use the concept constraint here
struct hash<T> {
  [[nodiscard]] size_t operator()(const T& obj) const noexcept {
    // Call the .hash() method which T should have (likely inherited)
    return obj.hash();
  }
};

template <PositionDerived T>
string to_string(const T& obj) {
  return "[" + to_string(obj.x) + "," + to_string(obj.y) + "," +
         to_string(obj.z) + "]";
}
}  // namespace std

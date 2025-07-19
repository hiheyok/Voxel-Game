// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <vector>

#include "Core/Position/PositionTypes.h"

// TODO(hiheyok): Refactor the code to use these
static constexpr int kEastDirection = 0;
static constexpr int kWestDirection = 1;
static constexpr int kUpDirection = 2;
static constexpr int kDownDirection = 3;
static constexpr int kSouthDirection = 4;
static constexpr int kNorthDirection = 5;
static constexpr int kAllDirections = 6;

static constexpr int kXAxis = 0;
static constexpr int kYAxis = 1;
static constexpr int kZAxis = 2;

// Use for offsets
template <class PositionType>
class Direction {
 public:
  constexpr Direction(int x, int y, int z, int direction) noexcept
      : offset_{x, y, z}, direction_{direction} {}

  constexpr Direction(const Direction&) noexcept = default;
  constexpr Direction(Direction&&) noexcept = default;
  constexpr ~Direction() noexcept = default;

  constexpr Direction& operator=(Direction&&) noexcept = default;
  constexpr Direction& operator=(const Direction&) noexcept = default;

  friend constexpr PositionType operator+(const PositionType& m,
                                          const Direction& n) {
    return m + n.offset_;
  }

  constexpr bool operator==(const Direction& other) const {
    return other.direction_ == direction_ && other.offset_ == offset_;
  }

  constexpr Direction operator!() const {
    Direction opposite{*this};
    opposite.offset_ *= -1;
    opposite.direction_ = GetOppositeDirection();
    return opposite;
  }

  constexpr int GetDirection() const { return direction_; }
  constexpr int GetOppositeDirection() const { return kOpposite[direction_]; }
  constexpr int GetAxis() const { return direction_ >> 1; }
  constexpr bool IsPositive() const { return (direction_ & 1) == 0; }
  constexpr bool IsNegative() const { return (direction_ & 1) == 1; }
  constexpr operator int() const { return direction_; }

 private:
  PositionType offset_;
  uint8_t direction_;
  static constexpr int kOpposite[6] = {1,0,3,2,5,4};
};

/*
 * Class containing directional offsets
 */
template <class PositionType>
class Directions {
 public:
  using const_iterator = const Direction<PositionType>*;

  static constexpr const_iterator begin() { return std::begin(kDirections); }

  static constexpr const_iterator end() { return std::end(kDirections); }

  static constexpr Direction<PositionType> kEast{1, 0, 0, kEastDirection};
  static constexpr Direction<PositionType> kWest{-1, 0, 0, kWestDirection};
  static constexpr Direction<PositionType> kUp{0, 1, 0, kUpDirection};
  static constexpr Direction<PositionType> kDown{0, -1, 0, kDownDirection};
  static constexpr Direction<PositionType> kSouth{0, 0, 1, kSouthDirection};
  static constexpr Direction<PositionType> kNorth{0, 0, -1, kNorthDirection};

  static constexpr int kXAxis = 0;
  static constexpr int kYAxis = 1;
  static constexpr int kZAxis = 2;

 private:
  static constexpr Direction<PositionType> kDirections[6]{
      kEast, kWest, kUp, kDown, kSouth, kNorth};
};

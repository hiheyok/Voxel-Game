#pragma once
#include <vector>
#include "Core/Position/ChunkPos.h"

// TODO: Refactor the code to use these
inline constexpr int kEastDirection = 0x00;
inline constexpr int kWestDirection = 0x01;
inline constexpr int kUpDirection = 0x02;
inline constexpr int kDownDirection = 0x03;
inline constexpr int kSouthDirection = 0x04;
inline constexpr int kNorthDirection = 0x05;

// Use for offsets
class Direction {
public:
    constexpr Direction(int x, int y, int z, int direction) noexcept :
        pos_{ x, y, z },
        direction_{ direction } {};

    constexpr Direction(const Direction&) noexcept = default;
    constexpr Direction(Direction&&) noexcept = default;
    constexpr ~Direction() noexcept = default;

    constexpr Direction& operator=(Direction&&) noexcept = default;
    constexpr Direction& operator=(const Direction&) noexcept = default;

    friend constexpr ChunkPos operator+(const ChunkPos& m, const Direction& n) {
        return m + n.pos_;
    }

    constexpr bool operator==(const Direction& other) const {
        return other.direction_ == direction_ && other.pos_ == pos_;
    }

    constexpr Direction operator!() const {
        Direction opposite{*this};
        opposite.pos_ *= -1;
        opposite.direction_ = GetOppositeDirection();
        return opposite;
    }

    constexpr int GetDirection() const {
        return direction_;
    }

    constexpr int GetOppositeDirection() const {
        return direction_ + 1 - 2 * (1 & direction_);
    }

    constexpr int GetAxis() const {
        return direction_ >> 1;
    }

    constexpr bool IsPositive() const {
        return (direction_ & 0b1) == 0;
    }

    constexpr bool IsNegative() const {
        return (direction_ & 0b1) == 1;
    }

    constexpr operator int() const {
        return direction_;
    }

private:
    ChunkPos pos_;
    int direction_;
};

/*
* Class containing directional offsets
*/
class Directions {
public:
    using const_iterator = const Direction*;

    static constexpr const_iterator begin() {
        return std::begin(kDirections);
    }

    static constexpr const_iterator end() {
        return std::end(kDirections);
    }

    static constexpr Direction kEast{ 1, 0, 0, kEastDirection };
    static constexpr Direction kWest{ -1, 0, 0, kWestDirection };
    static constexpr Direction kUp{ 0, 1, 0, kUpDirection };
    static constexpr Direction kDown{ 0, -1, 0, kDownDirection };
    static constexpr Direction kSouth{ 0, 0, 1, kSouthDirection };
    static constexpr Direction kNorth{ 0, 0, -1, kNorthDirection };

    static constexpr int kXAxis = 0;
    static constexpr int kYAxis = 1;
    static constexpr int kZAxis = 2;
private:
    static constexpr Direction kDirections[6]{ 
        { 1 , 0 , 0 , kEastDirection } , 
        { -1, 0 , 0 , kWestDirection }, 
        { 0 , 1 , 0 , kUpDirection },
        { 0 , -1, 0 , kDownDirection },
        { 0 , 0 , 1 , kSouthDirection },
        { 0 , 0 , -1, kNorthDirection }
    };
};
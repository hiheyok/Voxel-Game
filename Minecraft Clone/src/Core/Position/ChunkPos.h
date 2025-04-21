#pragma once

#ifndef CHUNK_POS_H
#define CHUNK_POS_H

#include <cmath>
#include <iostream>
#include <functional>
#include <cctype>
#include <string>

class ChunkPos {
public:
    int x, y, z;

    template <typename T>
    constexpr ChunkPos(T px, T py, T pz) : 
        x{ static_cast<int>(px) },
        y{ static_cast<int>(py) },
        z{ static_cast<int>(pz) } {}

    constexpr ChunkPos(int px, int py, int pz) :
        x{ px },
        y{ py },
        z{ pz } {
    }

    constexpr ChunkPos() = default;
    constexpr ChunkPos(const ChunkPos& m) = default;
    constexpr ~ChunkPos() = default;
    constexpr ChunkPos(ChunkPos&&) = default;

    constexpr void set(int px, int py, int pz) {
        x = px;
        y = py;
        z = pz;
    }

    void incrementSide(int side, int val) {
        reinterpret_cast<int*>(this)[side >> 1] += (1 - 2 * (side & 1)) * val;
    }

    int& operator[](int i) {
        return reinterpret_cast<int*>(this)[i];
    }

    constexpr void operator=(const ChunkPos& other) {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    constexpr ChunkPos operator-(const ChunkPos& other) const {
        return ChunkPos{ x - other.x, y - other.y, z - other.z };
    }

    constexpr ChunkPos operator+(const ChunkPos& other) const {
        return ChunkPos{ x + other.x, y + other.y, z + other.z };
    }

    template <typename T> 
    constexpr ChunkPos operator*(const T& other) const {
        return ChunkPos{ x * other, y * other, z * other };
    }

    template <typename T> // Division will floor the nums 
    constexpr ChunkPos operator/(const T& other) const {
        return ChunkPos{ floor((double)x / other), floor((double)y / other), floor((double)z / other) };
    }

    template <typename T>
    constexpr ChunkPos operator<<(const T& other) const {
        static_assert(std::is_integral<T>::value, "Integral required.");
        return ChunkPos{ x << other, y << other, z << other };
    }

    template <typename T>
    constexpr ChunkPos operator>>(const T& other) const {
        static_assert(std::is_integral<T>::value, "Integral required.");
        return ChunkPos{ x >> other, y >> other, z >> other };
    }

    template <typename T>
    constexpr ChunkPos operator&(const T& other) const {
        static_assert(std::is_integral<T>::value, "Integral required.");
        return ChunkPos{ x & other, y & other, z & other };
    }

    template <typename T>
    constexpr ChunkPos operator^(const T& other) const {
        static_assert(std::is_integral<T>::value, "Integral required.");
        return ChunkPos{ x ^ other, y ^ other, z ^ other };
    }

    template <typename T>
    constexpr ChunkPos operator|(const T& other) const {
        static_assert(std::is_integral<T>::value, "Integral required.");
        return ChunkPos{ x | other, y | other, z | other };
    }

    template <typename T>
    constexpr ChunkPos& operator<<=(const T& other) {
        static_assert(std::is_integral<T>::value, "Integral required.");
        x <<= other;
        y <<= other;
        z <<= other;
        return *this;
    }

    template <typename T>
    constexpr ChunkPos& operator>>=(const T& other) {
        static_assert(std::is_integral<T>::value, "Integral required.");
        x >>= other;
        y >>= other;
        z >>= other;
        return *this;
    }

    template <typename T>
    constexpr ChunkPos& operator&=(const T& other) {
        static_assert(std::is_integral<T>::value, "Integral required.");
        x &= other;
        x &= other;
        x &= other;
        return *this;
    }

    template <typename T>
    constexpr ChunkPos& operator^=(const T& other) {
        static_assert(std::is_integral<T>::value, "Integral required.");
        x ^= other;
        x ^= other;
        x ^= other;
        return *this;
    }

    template <typename T>
    constexpr ChunkPos& operator|=(const T& other) {
        static_assert(std::is_integral<T>::value, "Integral required.");
        x |= other;
        x |= other;
        x |= other;
        return *this;
    }

    template <typename T> 
    constexpr void operator*=(const T& other) {
        x *= other;
        y *= other;
        z *= other;
    }

    friend std::ostream& operator<<(std::ostream& os, const ChunkPos& m) {
        return os << '[' << m.x << ',' << m.y << ',' << m.z << ']';
    }

    friend std::string operator+(const char* str, const ChunkPos& m) {
        std::string s = std::string(str) + "[" + std::to_string(m.x) + "," + std::to_string(m.y) + "," + std::to_string(m.z) + "]";
        return s;
    }

    // for HashMaps and HashSets

    constexpr bool operator==(const ChunkPos& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    constexpr size_t hash() const {
        size_t h = 2166136261u;  // FNV offset basis
        h ^= static_cast<size_t>(x);
        h *= 16777619u;  // FNV prime
        h ^= static_cast<size_t>(y);
        h *= 16777619u;
        h ^= static_cast<size_t>(z);
        h *= 16777619u;
        return h;
    }

    operator std::string() const {
        return "[" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "]";
    }
};

namespace std {
    template< >
    struct hash<ChunkPos> {
        size_t operator()(const ChunkPos& obj) const {
            return obj.hash();
        }
    };

    string to_string(const ChunkPos& obj);
}

using RegionPos = ChunkPos;
using TallChunkPos = ChunkPos;
using ChunkColumnPos = ChunkPos;
using BlockPos = ChunkPos;
using LocalBlockPos = ChunkPos;

#endif
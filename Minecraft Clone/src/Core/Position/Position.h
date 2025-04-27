#ifndef CORE_POSITION_POSITION_H
#define CORE_POSITION_POSITION_H

#include <ostream>
#include <string>
#include <cmath>
#include <type_traits>
#include <cstddef>

template <typename Derived>
class Position {
    public:
        int x, y, z;
    
        template <typename T>
        constexpr Position(T px, T py, T pz) noexcept : 
            x{ static_cast<int>(px) },
            y{ static_cast<int>(py) },
            z{ static_cast<int>(pz) } {}
    
        constexpr Position(int px, int py, int pz) noexcept :
            x{ px },
            y{ py },
            z{ pz } {
        }
    
        constexpr Position() noexcept =  default;
        constexpr ~Position() = default;
    
        constexpr Position(const Position<Derived>& m) noexcept =  default;
        constexpr Position(Position<Derived>&&) noexcept =  default;
        constexpr Position& operator=(const Position<Derived>&) noexcept = default;
        constexpr Position& operator=(Position<Derived>&&) noexcept = default;
    
        constexpr void Set(int px, int py, int pz) noexcept {
            x = px;
            y = py;
            z = pz;
        }
    
        void IncrementSide(int side, int val) noexcept {
            const int direction = 1 - 2 * (side & 1);
            const int axis = side >> 1;
    
            (*this)[axis] += direction * val;
        }
    
        int& operator[](int axis) noexcept {
            return reinterpret_cast<int*>(this)[axis];
        }
    
        constexpr Derived operator-(const Derived& other) const {
            return Derived{ x - other.x, y - other.y, z - other.z };
        }
    
        constexpr Derived operator+(const Derived& other) const {
            return Derived{ x + other.x, y + other.y, z + other.z };
        }
    
        template <typename T> 
        constexpr Derived operator*(const T& other) const {
            return Derived{ x * other, y * other, z * other };
        }
    
        template <typename T> // Division will floor the nums 
        constexpr Derived operator/(const T& other) const {
            return Derived{ floor((double)x / other), floor((double)y / other), floor((double)z / other) };
        }
    
        template <typename T>
        constexpr Derived operator<<(const T& other) const {
            static_assert(std::is_integral<T>::value, "Integral required.");
            return Derived{ x << other, y << other, z << other };
        }
    
        template <typename T>
        constexpr Derived operator>>(const T& other) const {
            static_assert(std::is_integral<T>::value, "Integral required.");
            return Derived{ x >> other, y >> other, z >> other };
        }
    
        template <typename T>
        constexpr Derived operator&(const T& other) const {
            static_assert(std::is_integral<T>::value, "Integral required.");
            return Derived{ x & other, y & other, z & other };
        }
    
        template <typename T>
        constexpr Derived operator^(const T& other) const {
            static_assert(std::is_integral<T>::value, "Integral required.");
            return Derived{ x ^ other, y ^ other, z ^ other };
        }
    
        template <typename T>
        constexpr Derived operator|(const T& other) const {
            static_assert(std::is_integral<T>::value, "Integral required.");
            return Derived{ x | other, y | other, z | other };
        }
    
        template <typename T>
        constexpr Derived& operator<<=(const T& other) {
            static_assert(std::is_integral<T>::value, "Integral required.");
            x <<= other;
            y <<= other;
            z <<= other;
            return *this;
        }
    
        template <typename T>
        constexpr Derived& operator>>=(const T& other) {
            static_assert(std::is_integral<T>::value, "Integral required.");
            x >>= other;
            y >>= other;
            z >>= other;
            return *this;
        }
    
        template <typename T>
        constexpr Derived& operator&=(const T& other) {
            static_assert(std::is_integral<T>::value, "Integral required.");
            x &= other;
            y &= other;
            z &= other;
            return *this;
        }
    
        template <typename T>
        constexpr Derived& operator^=(const T& other) {
            static_assert(std::is_integral<T>::value, "Integral required.");
            x ^= other;
            y ^= other;
            z ^= other;
            return *this;
        }
    
        template <typename T>
        constexpr Derived& operator|=(const T& other) {
            static_assert(std::is_integral<T>::value, "Integral required.");
            x |= other;
            y |= other;
            z |= other;
            return *this;
        }
    
        template <typename T> 
        constexpr void operator*=(const T& other) {
            x *= other;
            y *= other;
            z *= other;
        }
    
        friend std::ostream& operator<<(std::ostream& os, const Position<Derived>& m) {
            return os << '[' << m.x << ',' << m.y << ',' << m.z << ']';
        }
    
        friend std::string operator+(const char* str, const Position<Derived>& m) {
            std::string s = std::string(str) + "[" + std::to_string(m.x) + "," + std::to_string(m.y) + "," + std::to_string(m.z) + "]";
            return s;
        }
    
        // for HashMaps and HashSets
    
        constexpr bool operator==(const Position<Derived>& other) const {
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

// Define a concept to identify types that follow the CRTP pattern with Position
template<typename T>
concept PositionDerived = std::is_base_of<Position<T>, T>::value;

namespace std {
    // Specialize std::hash<T> only for types T that satisfy PositionDerived
    template<PositionDerived T> // Use the concept constraint here
    struct hash<T> {
        size_t operator()(const T& obj) const noexcept {
             // Call the .hash() method which T should have (likely inherited)
            return obj.hash();
        }
    };

    template <PositionDerived T>
    string to_string(const T& obj) {
        return "[" + to_string(obj.x) + "," + to_string(obj.y) + "," + to_string(obj.z) + "]";
    }
} // namespace std

#endif
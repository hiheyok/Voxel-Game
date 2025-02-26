#pragma once

#ifndef CHUNK_POS_H
#define CHUNK_POS_H

#include <cmath>
#include <iostream>
#include <xhash>
#include <cctype>
#include <string>

#include "../../../Utils/LogUtils.h"

class ChunkPos {
	
	int data[3]{};

public:
	int& x = data[0];
	int& y = data[1];
	int& z = data[2];

	ChunkPos(int px, int py, int pz) {
		set(px, py, pz);
	}

	ChunkPos(const ChunkPos& m) {
		x = m.x;
		y = m.y;
		z = m.z;
	}

	ChunkPos() {

	}

	inline void set(int px, int py, int pz) {
		x = px;
		y = py;
		z = pz;
	}

	inline void incrementSide(int side, int val) {
		data[side >> 1] +=  (1 - (side & 0b1) * 2) * val;
	}

	inline int& operator[](int i) {
		return data[i];
	}

	inline void operator=(const ChunkPos& other) {
		memcpy(data, other.data, sizeof(int) * 3);
	}

	inline ChunkPos operator-(const ChunkPos& other) const {
		return ChunkPos{ x - other.x, y - other.y, z - other.z };
	}

	inline ChunkPos operator+(const ChunkPos& other) const {
		return ChunkPos{ x + other.x, y + other.y, z + other.z };
	}

	template <typename T> inline ChunkPos operator*(const T& other) const {
		return ChunkPos{ x * other, y * other, z * other };
	}

	template <typename T> inline ChunkPos operator/(const T& other) const {
		return ChunkPos{ x / other, y / other, z / other };
	}

	template <typename T> void operator*=(const T& other) {
		x *= other;
		y *= other;
		z *= other;
	}

	inline friend std::ostream& operator<<(std::ostream& os, const ChunkPos& m) {
		return os << '[' << m.x << ',' << m.y << ',' << m.z << ']';
	}

	inline friend std::string operator+(const char* str, const ChunkPos& m) {
		std::string s = std::string(str) + "[" + std::to_string(m.x) + "," + std::to_string(m.y) + "," + std::to_string(m.z) + "]";;
		return s;
	}

	// for HashMaps and HashSets

	inline bool operator==(const ChunkPos& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	inline size_t hash() const {
		constexpr uint64_t prime1 = 0x9e3779b97f4a7c15; // Large prime (Golden ratio scaled)
		constexpr uint64_t prime2 = 0xc6a4a7935bd1e995; // Another large prime

		size_t seed = std::hash<uint64_t>{}(x);
		seed ^= std::hash<uint64_t>{}(y)+prime1 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<uint64_t>{}(z)+prime2 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

namespace std {
	template<>
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

#endif
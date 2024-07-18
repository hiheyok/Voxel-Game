#pragma once
#include <string.h>

class ChunkLightingContainer { //Contains all lighting infomation for solid blocks
private:
	uint64_t data[256]{}; //x z y

public:

	static const unsigned char MaxLightLevel = 15;
	glm::ivec3 Position;

	ChunkLightingContainer() {
		ResetLighting();
	}

	inline uint64_t* getData() {
		return data;
	}

	inline void ReplaceData(uint64_t* src) {
		memcpy(data, src, sizeof(uint64_t) * 256);
	}

	inline void EditLight(int x, int y, int z, unsigned char LightingInfo) {
		data[(x << 4) | (z)] &= (~(0b1111ULL << (y << 2)));

		data[(x << 4) | (z)] |= ((uint64_t)LightingInfo << (y << 2));
	}

	inline uint8_t GetLighting(int x, int y, int z) {
		return (data[(x << 4) | (z)] >> (y << 2)) & 0b1111;
	}

	inline void ResetLighting() {
		memset((uint8_t*)data, MaxLightLevel | (MaxLightLevel << 4), 256 * 8); //8 = sizeof uint64_t
	}

	inline void ResetLightingCustom(uint8_t lvl) {
		memset((uint8_t*)data, lvl | (lvl << 4), 256 * 8); //8 = sizeof uint64_t
	}

};
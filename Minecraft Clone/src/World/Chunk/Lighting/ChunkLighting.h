#pragma once
#include <string.h>

class ChunkLightingContainer { //Contains all lighting infomation for solid blocks
private:
	uint64_t data[256]{}; //x z y

public:

	static const unsigned char MaxLightLevel = 15;

	ChunkLightingContainer() {
		ResetLighting();
	}

	void EditLight(int x, int y, int z, unsigned char LightingInfo) {
		data[(x << 4) | (z)] &= (~(0b1111ULL << (y << 2)));

		data[(x << 4) | (z)] |= ((uint64_t)LightingInfo << (y << 2));
	}

	unsigned char GetLighting(int x, int y, int z) {
		return (data[(x << 4) | (z)] >> (y << 2)) & 0b1111;
	}

	void ResetLighting() {
		memset((uint8_t*)data, MaxLightLevel | (MaxLightLevel << 4), 256 * 8); //8 = sizeof uint64_t
	}

};
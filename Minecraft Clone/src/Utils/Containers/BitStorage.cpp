#include "BitStorage.h"


void BitStorage3D::Initialize(int x, int y, int z) { //Set Sizes
	int nBits = x * y * z;

	int mapsize = nBits >> 6;

	if (nBits & 0b111111) {
		mapsize++;
	}

	map.resize(mapsize);
}

void BitStorage3D::ClearBit(int x, int y, int z) {
	int index = sx * (sy * sz) + sy * (sz)+sz;

	size_t NumIndex = index >> 6;
	size_t Index = index & 0b111111;

	map[NumIndex] &= (~(0b1LL << Index));
}

void BitStorage3D::clear() {
	for (int i = 0; i < map.size(); i++) {
		map[i] = 0;
	}
}
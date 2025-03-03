#include "BitStorage.h"


void BitStorage3D::Initialize(int x, int y, int z) { //Set Sizes
	int nBits = x * y * z;

	int mapsize = nBits >> 6;

	if (nBits & 0b111111) {
		mapsize++;
	}

	map_.resize(mapsize);
}

void BitStorage3D::ClearBit(int x, int y, int z) {
	int index = sx_ * (sy_ * sz_) + sy_ * (sz_)+sz_;

	size_t NumIndex = index >> 6;
	size_t Index = index & 0b111111;

	map_[NumIndex] &= (~(0b1LL << Index));
}

void BitStorage3D::Clear() {
	for (int i = 0; i < map_.size(); i++) {
		map_[i] = 0;
	}
}
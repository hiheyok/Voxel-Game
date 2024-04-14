#pragma once
#include <vector>

class BitStorage3D {
public:
	void Initialize(int x, int y, int z) { //Set Sizes
		int nBits = x * y * z;

		int mapsize = nBits >> 6;

		if (nBits & 0b111111) {
			mapsize++;
		}

		map.resize(mapsize);
	}

	void SetBit(int x, int y, int z) {
		int index = sx * (sy * sz) + sy * (sz)+sz;

		size_t NumIndex = index >> 6;
		size_t Index = index & 0b111111;

		map[NumIndex] |= (0b1LL << Index);
	}

	inline bool GetBit(int x, int y, int z) {
		int index = sx * (sy * sz) + sy * (sz)+sz;

		size_t NumIndex = index >> 6;
		size_t Index = index & 0b111111;
		return (map[NumIndex] >> Index) & 0b1;
	}

	inline void ClearBit(int x, int y, int z) {
		int index = sx * (sy * sz) + sy * (sz)+sz;

		size_t NumIndex = index >> 6;
		size_t Index = index & 0b111111;

		map[NumIndex] &= (~(0b1LL << Index));
	}

	inline void clear() {
		for (int i = 0; i < map.size(); i++) {
			map[i] = 0;
		}
	}
private:
	std::vector<uint64_t> map;
	int sx = 0, sy = 0, sz = 0;
};

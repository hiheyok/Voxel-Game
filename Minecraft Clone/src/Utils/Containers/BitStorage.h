#pragma once
#include <vector>

class BitStorage3D {
public:
	void Initialize(int x, int y, int z);

	inline void SetBit(int x, int y, int z) {
		int index = sx * (sy * sz) + sy * (sz)+sz;

		size_t NumIndex = index >> 6;
		size_t Index = index & 0b111111;

		map[NumIndex] |= (0b1LL << Index);
	}

	inline void SetBit(int* pos) {
		SetBit(pos[0], pos[1], pos[2]);
	}

	inline bool GetBit(int x, int y, int z) {
		int index = sx * (sy * sz) + sy * (sz)+sz;

		size_t NumIndex = index >> 6;
		size_t Index = index & 0b111111;
		return (map[NumIndex] >> Index) & 0b1;
	}

	inline bool GetBit(int* pos) {
		return GetBit(pos[0], pos[1], pos[2]);
	}

	inline void ClearBit(int x, int y, int z);

	inline void clear();
private:
	std::vector<uint64_t> map;
	int sx = 0, sy = 0, sz = 0;
};

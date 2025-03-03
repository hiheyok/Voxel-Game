#pragma once
#include <vector>

class BitStorage3D {
public:
	void Initialize(int x, int y, int z);

	inline void SetBit(int x, int y, int z) {
		int index = sx_ * (sy_ * sz_) + sy_ * (sz_)+sz_;

		size_t NumIndex = index >> 6;
		size_t Index = index & 0b111111;

		map_[NumIndex] |= (0b1LL << Index);
	}

	inline void SetBit(int* pos) {
		SetBit(pos[0], pos[1], pos[2]);
	}

	inline bool GetBit(int x, int y, int z) {
		int index = sx_ * (sy_ * sz_) + sy_ * (sz_)+sz_;

		size_t numIndex = index >> 6;
		size_t bitIndex = index & 0b111111;
		return (map_[numIndex] >> bitIndex) & 0b1;
	}

	inline bool GetBit(int* pos) {
		return GetBit(pos[0], pos[1], pos[2]);
	}

	inline void ClearBit(int x, int y, int z);

	inline void Clear();
private:
	std::vector<uint64_t> map_;
	int sx_ = 0, sy_ = 0, sz_ = 0;
};

#pragma once
#include <vector>

class BitStorage3D {
public:
	void Initialize(int x, int y, int z);

	void SetBit(int x, int y, int z);

	inline bool GetBit(int x, int y, int z);

	inline void ClearBit(int x, int y, int z);

	inline void clear();
private:
	std::vector<uint64_t> map;
	int sx = 0, sy = 0, sz = 0;
};

#pragma once
#include <vector>
/*

heightmap for chunk column 32 * 16 = 512 blocks columns 9 bits per column

16 x 16 * 9 = 2304 bits = 36 uint64_t
*/

class Heightmap {
private:
	std::vector<uint16_t> data;
	bool isInit = false;
public:
	Heightmap() {
	}

	void edit(int x, int z, int height);

	uint16_t get(int x, int z) const;

	void init();
	void Clear();

	std::vector<uint16_t> getData() const;
};

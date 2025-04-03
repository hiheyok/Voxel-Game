#pragma once
#include <vector>
/*

heightmap for chunk column 32 * 16 = 512 blocks columns 9 bits per column

16 x 16 * 9 = 2304 bits = 36 uint64_t
*/

class Heightmap {
public:
    Heightmap();
    ~Heightmap();

    void Edit(int x, int z, int height);

    int16_t Get(int x, int z) const;

    void Clear();
private:
    std::vector<char> data_;
};

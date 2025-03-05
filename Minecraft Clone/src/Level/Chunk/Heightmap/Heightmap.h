#pragma once
#include <vector>
/*

heightmap for chunk column 32 * 16 = 512 blocks columns 9 bits per column

16 x 16 * 9 = 2304 bits = 36 uint64_t
*/

class Heightmap {
private:
    std::vector<uint16_t> data_;
    bool is_init_ = false;
public:
    Heightmap() {
    }

    void Edit(int x, int z, int height);

    uint16_t Get(int x, int z) const;

    void Init();
    void Clear();

    std::vector<uint16_t> GetData() const;
};

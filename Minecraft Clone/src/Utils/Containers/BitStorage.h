#pragma once
#include <vector>

class BitStorage3D {
public:
    void Initialize(int x, int y, int z);

    void SetBit(int x, int y, int z);

    void SetBit(int* pos);

    bool GetBit(int x, int y, int z);

    bool GetBit(int* pos);

    void ClearBit(int x, int y, int z);

    void Clear();
private:
    std::vector<uint64_t> map_;
    int sx_ = 0, sy_ = 0, sz_ = 0;
};

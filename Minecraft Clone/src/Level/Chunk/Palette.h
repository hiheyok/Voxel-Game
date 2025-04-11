#pragma once
#include <vector>

#include "Core/Typenames.h"
#include "Utils/Containers/NBitVector.h"
#include "Level/Block/Blocks.h"
/*
* The palette class is used to store the block data in a compressed state in a chunk
* Each chunk has up to 4096 unique blocks so it attempts to store it in up to 12 bits per blocks
 */

//TODO: for a large amount of unique block, use stack / hashmap
class Palette { 
public:
    Palette();
    ~Palette();
    Palette(Palette&&) noexcept;
    Palette(const Palette&) noexcept;
    Palette& operator=(const Palette&);
    Palette& operator=(Palette&&) noexcept;

    BlockID GetBlock(int x, int y, int z) const;
    BlockID GetBlockUnsafe(int x, int y, int z) const;

    void SetBlock(BlockID block, int x, int y, int z);

    void SetBlockUnsafe(BlockID block, int x, int y, int z);
private:

    using PaletteIndex = uint16_t;
    using StorageBit = uint64_t;

    static constexpr int kMinBitWidth = 1; // Minimum bits
    static constexpr int kMaxBitWidth = 12;

    int current_bit_width_;
    int unique_blocks_count_ = 1; // Initialize with only air blocks

    int d1 = 0, d2 = 0;

    NBitVector<StorageBit> data_;

    // BlockID -> Block, int_16 -> num of that block
    std::vector<std::pair<BlockID, int16_t>> palette_entries_;

    static constexpr int GetBitWidth(unsigned int n) {
        int bitWidth = std::bit_width(n);
        return std::max(bitWidth, kMinBitWidth);
    }

    static constexpr size_t GetIndex(int x, int y, int z) {
        return x * kChunkDim * kChunkDim + y * kChunkDim + z;
    }

    void Shrink();

    void Grow();

    void Resize();

    PaletteIndex GetOrAddPaletteIndex(BlockID block);
};
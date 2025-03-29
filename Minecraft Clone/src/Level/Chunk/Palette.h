#pragma once
#include <vector>
#include "../Typenames.h"
#include "../../Utils/Containers/NBitVector.h"
#include "Block/Blocks.h"
/*
* The palette class is used to store the block data in a compressed state in a chunk
* Each chunk has up to 4096 unique blocks so it attempts to store it in up to 12 bits per blocks
 */

//TODO: for a large amount of unique block, use stack / hashmap
class Palette { 
private:

    using PaletteIndex = uint16_t;
    using StorageBit = uint64_t;

    static constexpr int CHUNK_DIM = 16;
    static constexpr int CHUNK_SIZE = CHUNK_DIM * CHUNK_DIM * CHUNK_DIM; // 4096
    static constexpr int MIN_BIT_WIDTH = 1; // Minimum bits

    static constexpr int MAX_BIT_WIDTH = 12;

    int current_bit_width_;
    int unique_blocks_count_ = 1; // Initialize with only air blocks

    int d1 = 0, d2 = 0;

    NBitVector<StorageBit> data_;

    // BlockID -> Block, int_16 -> num of that block
    std::vector<std::pair<BlockID, int16_t>> palette_entries_;

    constexpr static int GetBitWidth(unsigned int n) {
        int bitWidth = std::bit_width(n);
        return std::max(bitWidth, MIN_BIT_WIDTH);
    }

    constexpr static size_t GetIndex(int x, int y, int z) {
        return x * CHUNK_DIM * CHUNK_DIM + y * CHUNK_DIM + z;
    }

    void Shrink() {
        int newBitWidth = GetBitWidth(unique_blocks_count_);
        NBitVector<StorageBit> newData(CHUNK_SIZE, newBitWidth);

        // Repack

        std::vector<std::pair<BlockID, int16_t>> newBlockToPaletteIndex(unique_blocks_count_, { 0, 0 });
        std::vector<PaletteIndex> newPaletteIndex(palette_entries_.size(), 0);

        // 2 pointers

        int curr = 0;
        for (int i = 0; i < palette_entries_.size(); ++i) {
            if (palette_entries_[i].second == 0) continue;
            newBlockToPaletteIndex[curr] = palette_entries_[i];
            newPaletteIndex[i] = curr;
            curr++;
        }

        for (int i = 0; i < CHUNK_SIZE; ++i) {
            PaletteIndex currVal = data_.Get(i);
            PaletteIndex newVal = newPaletteIndex[currVal];
            newData.Set(i, newVal);
        }

        data_ = newData;
        current_bit_width_ = newBitWidth;

        // refactor palette block index
        palette_entries_ = newBlockToPaletteIndex;
    }

    void Grow() {
        int newBitWidth = GetBitWidth(unique_blocks_count_);
        NBitVector<StorageBit> newData(CHUNK_SIZE, newBitWidth);
        current_bit_width_ = newBitWidth;
        for (int i = 0; i < CHUNK_SIZE; ++i) {
            newData.Set(i, data_.Get(i));
        }

        data_ = newData;
    }

    void Resize() {
        int newBitWidth = GetBitWidth(unique_blocks_count_);
        // Plus 1 to avoid frequent resizing
        if (newBitWidth == current_bit_width_ || newBitWidth + 1 == current_bit_width_) return;

        if (newBitWidth + 1 < current_bit_width_) { 
            Shrink();
        }
        else {
            Grow();
        }
    }

    PaletteIndex GetOrAddPaletteIndex(BlockID block) {
        int firstZero = -1;
        for (PaletteIndex i = 0; i < palette_entries_.size(); i++) {
            if (palette_entries_[i].first == block) {
                return i;
            }
            if (palette_entries_[i].second == 0) {
                firstZero = i;
            }
        }

        //d1++;
        
        if (firstZero != -1) {
            palette_entries_[firstZero].first = block;
            return firstZero;
        }
        //d2++;

        palette_entries_.emplace_back(block, 0);
        Resize();
        PaletteIndex newIndex = palette_entries_.size() - 1;

        return newIndex;
    }

public:
    Palette() : current_bit_width_{MIN_BIT_WIDTH} , data_{ CHUNK_SIZE, current_bit_width_ } {
        palette_entries_.emplace_back(g_blocks.AIR, CHUNK_SIZE);
    }
    ~Palette() = default;
    Palette(Palette&&) noexcept = default;
    Palette& operator=(const Palette&) = default;
    Palette& operator=(Palette&&) noexcept = default;

    BlockID GetBlock(int x, int y, int z) const {
        if (x < 0 || x >= CHUNK_DIM || y < 0 || y >= CHUNK_DIM || z < 0 || z >= CHUNK_DIM) {
            throw std::out_of_range("Palette::GetBlock - Invalid palette index");
        }

        PaletteIndex idx = data_.Get(GetIndex(x, y, z));

        if (idx >= palette_entries_.size()) throw std::runtime_error("Palette::GetBlock - Error in getting BlockID");

        return palette_entries_[idx].first;
    }

    BlockID GetBlockUnsafe(int x, int y, int z) const {
        PaletteIndex idx = data_.GetUnsafe(GetIndex(x, y, z));
        return palette_entries_[idx].first;
    }

    void SetBlock(BlockID block, int x, int y, int z) {
        if (x < 0 || x >= CHUNK_DIM || y < 0 || y >= CHUNK_DIM || z < 0 || z >= CHUNK_DIM) {
            throw std::out_of_range("Palette::GetBlock - Invalid palette index");
        }

        // Look at original block
        PaletteIndex oldPaletteIdx = data_.Get(GetIndex(x, y, z));
        if (oldPaletteIdx >= palette_entries_.size() || palette_entries_[oldPaletteIdx].second <= 0) {
            throw std::runtime_error("Palette::SetBlock - Corrupt old palette index found in data.");
        }

        palette_entries_[oldPaletteIdx].second--;

        BlockID oldBlockId = palette_entries_[oldPaletteIdx].first;

        bool uniqueCountChanged = false;
        if (block == oldBlockId) {
            return;
        }

        if (palette_entries_[oldPaletteIdx].second == 0) {
            unique_blocks_count_--;
            uniqueCountChanged = !uniqueCountChanged;
        }

        PaletteIndex idx = GetOrAddPaletteIndex(block);
        if (palette_entries_[idx].second++ == 0) {
            unique_blocks_count_++;
            uniqueCountChanged = !uniqueCountChanged;
        }

        data_.Set(GetIndex(x, y, z), idx);
        if (uniqueCountChanged) {
            Resize();
        }
    }

    void SetBlockUnsafe(BlockID block, int x, int y, int z) {
        // Look at original block
        PaletteIndex oldPaletteIdx = data_.GetUnsafe(GetIndex(x, y, z));
        if (oldPaletteIdx >= palette_entries_.size() || palette_entries_[oldPaletteIdx].second <= 0) {
            throw std::runtime_error("Palette::SetBlock - Corrupt old palette index found in data.");
        }
        palette_entries_[oldPaletteIdx].second--;
        BlockID oldBlockId = palette_entries_[oldPaletteIdx].first;
        bool uniqueCountChanged = false;
        if (block == oldBlockId) {
            return;
        }

        if (palette_entries_[oldPaletteIdx].second == 0) {
            unique_blocks_count_--;
            uniqueCountChanged = !uniqueCountChanged;
        }

        PaletteIndex idx = GetOrAddPaletteIndex(block);
        if (palette_entries_[idx].second++ == 0) {
            unique_blocks_count_++;
            uniqueCountChanged = !uniqueCountChanged;
        }

        data_.Set(GetIndex(x, y, z), idx);
        if (uniqueCountChanged) {
            Resize();
        }
    }
};
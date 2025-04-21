#include "Level/Chunk/Palette.h"

Palette::Palette() : current_bit_width_{ kMinBitWidth }, data_{ kChunkSize3D, current_bit_width_ } {
    palette_entries_.emplace_back(g_blocks.AIR, static_cast<uint16_t>(kChunkSize3D));
}

Palette::~Palette() = default;
Palette::Palette(Palette&&) noexcept = default;
Palette::Palette(const Palette&) noexcept = default;
Palette& Palette::operator=(const Palette&) = default;
Palette& Palette::operator=(Palette&&) noexcept = default;

void Palette::Shrink() {
    int newBitWidth = GetBitWidth(unique_blocks_count_);
    NBitVector<StorageBit> newData(kChunkSize3D, newBitWidth);

    // Repack

    std::vector<std::pair<BlockID, int16_t>> newBlockToPaletteIndex(unique_blocks_count_);
    std::vector<PaletteIndex> newPaletteIndex(palette_entries_.size(), 0);

    // 2 pointers

    PaletteIndex curr = 0;
    for (int i = 0; i < palette_entries_.size(); ++i) {
        if (palette_entries_[i].second == 0) continue;
        newBlockToPaletteIndex[curr] = palette_entries_[i];
        newPaletteIndex[i] = curr;
        curr++;
    }

    for (int i = 0; i < kChunkSize3D; ++i) {
        PaletteIndex currVal = static_cast<PaletteIndex>(data_.Get(i));
        PaletteIndex newVal = newPaletteIndex[currVal];
        newData.Set(i, newVal);
    }

    data_ = newData;
    current_bit_width_ = newBitWidth;

    // refactor palette block index
    palette_entries_ = newBlockToPaletteIndex;
}

void Palette::Grow() {
    int newBitWidth = GetBitWidth(unique_blocks_count_);
    NBitVector<StorageBit> newData(kChunkSize3D, newBitWidth);
    current_bit_width_ = newBitWidth;
    for (int i = 0; i < kChunkSize3D; ++i) {
        newData.Set(i, data_.Get(i));
    }

    data_ = newData;
}

void Palette::Resize() {
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

Palette::PaletteIndex Palette::GetOrAddPaletteIndex(BlockID block) {
    PaletteIndex firstZero = 0;
    bool foundZero = false;
    for (PaletteIndex i = 0; i < palette_entries_.size(); i++) {
        if (palette_entries_[i].first == block) {
            return i;
        }
        if (palette_entries_[i].second == 0) {
            foundZero = true;
            firstZero = i;
        }
    }

    //d1++;

    if (foundZero) {
        palette_entries_[firstZero].first = block;
        return firstZero;
    }
    //d2++;

    palette_entries_.emplace_back(block, static_cast<int16_t>(0));
    Resize();
    PaletteIndex newIndex = static_cast<PaletteIndex>(palette_entries_.size() - 1);

    return newIndex;
}

BlockID Palette::GetBlock(int x, int y, int z) const {
    if (x < 0 || x >= kChunkDim || y < 0 || y >= kChunkDim || z < 0 || z >= kChunkDim) {
        throw std::out_of_range("Palette::GetBlock - Invalid palette index");
    }

    PaletteIndex idx = static_cast<PaletteIndex>(data_.Get(GetIndex(x, y, z)));

    if (idx >= palette_entries_.size()) throw std::runtime_error("Palette::GetBlock - Error in getting BlockID");

    return palette_entries_[idx].first;
}

BlockID Palette::GetBlockUnsafe(int x, int y, int z) const {
    PaletteIndex idx = static_cast<PaletteIndex>(data_.GetUnsafe(GetIndex(x, y, z)));
    return palette_entries_[idx].first;
}

void Palette::SetBlock(BlockID block, int x, int y, int z) {
    if (x < 0 || x >= kChunkDim || y < 0 || y >= kChunkDim || z < 0 || z >= kChunkDim) {
        throw std::out_of_range("Palette::SetBlock - Invalid palette index");
    }

    // Look at original block
    PaletteIndex oldPaletteIdx = static_cast<PaletteIndex>(data_.Get(GetIndex(x, y, z)));

    if (oldPaletteIdx >= palette_entries_.size() || palette_entries_[oldPaletteIdx].second <= 0) {
        throw std::runtime_error("Palette::SetBlock - Corrupt old palette index found in data.");
    }

    BlockID oldBlockId = palette_entries_[oldPaletteIdx].first;

    if (block == oldBlockId) {
        return;
    }
    bool uniqueCountChanged = false;

    palette_entries_[oldPaletteIdx].second--;
    if (palette_entries_[oldPaletteIdx].second == 0) {
        unique_blocks_count_--;
        uniqueCountChanged = true;
    }

    PaletteIndex idx = GetOrAddPaletteIndex(block);
    if (palette_entries_[idx].second == 0) {
        unique_blocks_count_++;
        uniqueCountChanged = !uniqueCountChanged;
    }
    palette_entries_[idx].second++;

    data_.Set(GetIndex(x, y, z), idx);
    if (uniqueCountChanged) {
        Resize();
    }
}

void Palette::SetBlockUnsafe(BlockID block, int x, int y, int z) {
    // Look at original block
    PaletteIndex oldPaletteIdx = static_cast<PaletteIndex>(data_.GetUnsafe(GetIndex(x, y, z)));
    if (oldPaletteIdx >= palette_entries_.size() || palette_entries_[oldPaletteIdx].second <= 0) {
        throw std::runtime_error("Palette::SetBlockUnsafe - Corrupt old palette index found in data.");
    }
    BlockID oldBlockId = palette_entries_[oldPaletteIdx].first;
    bool uniqueCountChanged = false;
    if (block == oldBlockId) {
        return;
    }
    palette_entries_[oldPaletteIdx].second--;

    if (palette_entries_[oldPaletteIdx].second == 0) {
        unique_blocks_count_--;
        uniqueCountChanged = !uniqueCountChanged;
    }

    PaletteIndex idx = GetOrAddPaletteIndex(block);
    if (palette_entries_[idx].second++ == 0) {
        unique_blocks_count_++;
        uniqueCountChanged = !uniqueCountChanged;
    }

    data_.SetUnsafe(GetIndex(x, y, z), idx);
    if (uniqueCountChanged) {
        Resize();
    }
}
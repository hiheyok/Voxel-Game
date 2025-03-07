#include "ChunkColumn.h"

Heightmap& ChunkColumn::GetHeightmap() {
    return column_heightmap_;
}

void ChunkColumn::ReplaceLightContainer(int y, std::unique_ptr<ChunkLightingContainer> c) {
    if (column_[y] == nullptr) {
        throw std::exception("Couldn't place lighting");
    }

    column_[y]->lighting_->ReplaceData(c->getData());
}

ChunkColumn::ChunkColumn() {
    column_.resize(32);
    light_dirty_.resize(32, false);
    column_heightmap_.Init();
}

void ChunkColumn::AddChunk(std::unique_ptr<Chunk> chunk, int relativeHeightLevel) {
    column_[relativeHeightLevel] = std::move(chunk);
    UpdateHeightmap(relativeHeightLevel);
}

void ChunkColumn::UpdateChunk(int relativeHeightLevel) {
    UpdateHeightmap(relativeHeightLevel);
}

Chunk* ChunkColumn::GetChunk(int heightLevel) const {
    return column_[heightLevel].get();
}

int16_t ChunkColumn::FindSurfaceHeight(uint8_t x, uint8_t z, uint8_t startingChunk) const {
    for (int16_t currChunk = startingChunk; currChunk >= 0; --currChunk) {
        const std::unique_ptr<Chunk>& curr = column_[currChunk];

        if (curr == nullptr) continue;

        for (int y = 15; y >= 0; y--) {
            if (curr->GetBlockUnsafe(x, y, z) != g_blocks.AIR) {
                return currChunk * 16 + y;
            }
        }
    }
    return -1;
}

int16_t ChunkColumn::FindSurfaceHeightSingleChunk(uint8_t height, uint8_t x, uint8_t z) const {
    const std::unique_ptr<Chunk>& chunk = column_[height];
    if (chunk == nullptr) return -1;
    for (int y = 15; y >= 0; y--) {
        if (chunk->GetBlockUnsafe(x, y, z) != g_blocks.AIR) return y;
    }
    return -1;
}

void ChunkColumn::UpdateHeightmapSingleBlock(int height, BlockID block, uint8_t x, uint8_t y, uint8_t z) {
    int currHeight = column_heightmap_.Get(z, y);

    //tmp
    for (int i = 0; i <= height; i++) {
        if (column_[i] != nullptr) {
            light_dirty_[i] = true;
        }
    }

    if (height * 16 + y < currHeight) {
        return;
    }

    if (block == g_blocks.AIR) {
        if (currHeight == height) {
            int16_t surfaceLevel = FindSurfaceHeight(x, z, height);
            if (surfaceLevel == -1) {
                surfaceLevel = 0;
            }
            column_heightmap_.Edit(x, z, surfaceLevel);
        }
    }
    else {
        column_heightmap_.Edit(x, z, height * 16 + y);
    }
}

void ChunkColumn::UpdateHeightmap(int height) {
    for (uint8_t x = 0; x < 16; x++) {
        for (uint8_t z = 0; z < 16; z++) {
            int16_t surfaceLevel = FindSurfaceHeight(x, z, static_cast<uint8_t>(height));
            surfaceLevel = (surfaceLevel != -1) * surfaceLevel;
            column_heightmap_.Edit(x, z, surfaceLevel);
        }
    }

    for (int i = 0; i <= height; i++) {
        if (column_[i] != nullptr) {
            light_dirty_[i] = true;
        }
    }
}
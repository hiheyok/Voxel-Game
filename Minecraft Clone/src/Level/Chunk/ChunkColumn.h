#pragma once
#include "Chunk.h"
#include "Lighting/ChunkLighting.h"
#include "../../Utils/Containers/BitStorage.h"
#include "Heightmap/Heightmap.h"
#include <glm/vec2.hpp>
#include <intrin.h>

// TODO: Rework this refactor ChunkColumnPos system
class ChunkColumn { //Helps with stuff like lighting
private:
    std::vector<Chunk*> column_;
    std::vector<std::shared_ptr<ChunkLightingContainer>> light_column_;
    Heightmap column_heightmap_;
    // glm::ivec2 Position;
public:
    std::vector<bool> light_dirty_;

    Heightmap& GetHeightmap() {
        return column_heightmap_;
    }

    void ReplaceLightContainer(int y, std::shared_ptr<ChunkLightingContainer> c) {
        if (light_column_[y] == nullptr) {
            light_column_[y] = std::move(c);
            return;
        }

        light_column_[y]->ReplaceData(c->getData());
    }

    ChunkColumn() {
        column_.resize(32, nullptr);
        light_column_.resize(32, nullptr);
        light_dirty_.resize(32, false);
        column_heightmap_.Init();
    }

    void AddChunk(Chunk* chunk, int relativeHeightLevel) {
        column_[relativeHeightLevel] = chunk;
        light_column_[relativeHeightLevel] = chunk->lighting_;
        UpdateHeightmap(relativeHeightLevel);
    }

    void UpdateChunk(int relativeHeightLevel) {
        UpdateHeightmap(relativeHeightLevel);
    }

    Chunk* GetChunk(int heightLevel) const {
        return column_[heightLevel];
    }

    int16_t FindSurfaceHeight(uint8_t x, uint8_t z, uint8_t startingChunk = 31) const {
        for (int currChunk = startingChunk;  currChunk >= 0; --currChunk) {
            Chunk* curr = column_[currChunk];
            
            if (curr == nullptr) continue;

            for (int y = 15; y >= 0; y--) {
                if (curr->GetBlockUnsafe(x, y, z) != g_blocks.AIR) {
                    return currChunk * 16 + y;
                }
            }
        }
        return -1;
    }

    /*
    Input is the y axis where the chunk is located at and the x and z block position relative to the chunk
    */

    int16_t FindSurfaceHeightSingleChunk(uint8_t height, uint8_t x, uint8_t z) const {
        Chunk* currChunk = column_[height];
        if (currChunk == nullptr) return -1;
        for (int y = 15; y >= 0; y--) {
            if (currChunk->GetBlockUnsafe(x, y, z) != g_blocks.AIR) return y;
        }
        return -1;
    }

    void UpdateHeightmapSingleBlock(int height, BlockID block, uint8_t x, uint8_t y, uint8_t z) {
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
        } else {
            column_heightmap_.Edit(x, z, height * 16 + y);
        }
    }

    void UpdateHeightmap(uint16_t height) {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
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
}; 

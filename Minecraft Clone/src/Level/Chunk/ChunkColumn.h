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
    std::vector<std::unique_ptr<Chunk>> column_;
    Heightmap column_heightmap_;
    // glm::ivec2 Position;
public:
    std::vector<bool> light_dirty_;

    Heightmap& GetHeightmap();

    void ReplaceLightContainer(int y, std::unique_ptr<ChunkLightingContainer> c);

    ChunkColumn();

    void AddChunk(std::unique_ptr<Chunk> chunk, int relativeHeightLevel);

    void UpdateChunk(int relativeHeightLevel);

    Chunk* GetChunk(int heightLevel) const;

    int16_t FindSurfaceHeight(uint8_t x, uint8_t z, uint8_t startingChunk = 31) const;

    /*
    Input is the y axis where the chunk is located at and the x and z block position relative to the chunk
    */

    int16_t FindSurfaceHeightSingleChunk(uint8_t height, uint8_t x, uint8_t z) const;

    void UpdateHeightmapSingleBlock(int height, BlockID block, uint8_t x, uint8_t y, uint8_t z);

    void UpdateHeightmap(int height);
}; 

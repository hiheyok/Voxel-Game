#pragma once
#include <glm/vec2.hpp>

#include "Level/Chunk/Chunk.h"
#include "Level/Chunk/Lighting/LightStorage.h"
#include "Utils/Containers/BitStorage.h"
#include "Level/Chunk/Heightmap/Heightmap.h"

// TODO: Rework this refactor ChunkColumnPos system
class ChunkColumn { //Helps with stuff like lighting
public:
    std::vector<bool> light_dirty_;

    Heightmap& GetHeightmap();

    void ReplaceLightContainer(int y, std::unique_ptr<LightStorage> c);

    ChunkColumn();

    void AddChunk(std::unique_ptr<Chunk> chunk, int relativeHeightLevel);

    void UpdateChunk(int relativeHeightLevel);

    Chunk* GetChunk(int heightLevel) const;

    int16_t FindSurfaceHeight(int x, int z, int startingChunk = 31) const;

    /*
    Input is the y axis where the chunk is located at and the x and z block position relative to the chunk
    */

    int16_t FindSurfaceHeightSingleChunk(int height, int x, int z) const;

    void UpdateHeightmapSingleBlock(int height, BlockID block, int x, int y, int z);

    void UpdateHeightmap(int height);
private:
    std::vector<std::unique_ptr<Chunk>> column_;
    Heightmap column_heightmap_;
    // glm::ivec2 Position;
}; 

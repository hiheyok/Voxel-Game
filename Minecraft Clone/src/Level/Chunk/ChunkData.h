#pragma once

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <glm/vec3.hpp>

#include "Core/Typenames.h"
#include "Level/TerrainGeneration/Structures/Structure.h"
#include "Level/Chunk/Lighting/LightStorage.h"
#include "Level/Chunk/ChunkPos/ChunkPos.h"
#include "Level/Chunk/Palette.h"

class Heightmap;

struct ChunkRawData;

class ChunkContainer {
public:
    ChunkPos position_;
    std::vector<SetBlockRelative> outside_block_to_place_[6]{};
    ChunkContainer* neighbors_[6]{ nullptr };
    std::unique_ptr<LightStorage> lighting_;
    std::unique_ptr<Heightmap> heightmap_;
    std::atomic<bool> is_empty_ = true;

    ChunkContainer();
    ~ChunkContainer();
    ChunkContainer(const ChunkRawData&);
    
    void SetNeighbor(ChunkContainer* neighbor, unsigned int side);
    ChunkContainer* GetNeighbor(unsigned int side) const;
    void ClearNeighbors();

    BlockID GetBlock(int x, int y, int z) const;
    BlockID GetBlockUnsafe(int x, int y, int z) const;

    void SetBlock(BlockID block, int x, int y, int z);
    void SetBlockUnsafe(BlockID block, int x, int y, int z);

    void SetData(const ChunkRawData& data);
    ChunkRawData GetRawData();
    LightStorage GetLightData();

    void SetPosition(int x, int y, int z);

    void UpdateHeightMap();
    void UpdateHeightMap(int x, int z);

    // Check light update request

    bool CheckLightDirty();
private:
    std::atomic<bool> in_use_ = false;
    bool light_dirty_ = false;
    Palette block_storage_;
};

#endif
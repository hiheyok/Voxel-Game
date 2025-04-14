#pragma once

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <glm/vec3.hpp>
#include <vector>

#include "Core/Typenames.h"
#include "Level/Chunk/Palette.h"
#include "Level/Light/LightStorage.h"

class Heightmap;
class LightStorage;

struct ChunkRawData;
struct SetBlockRelative;

class ChunkContainer {
public:
    ChunkPos position_;
    std::vector<ChunkContainer*> neighbors_;
    std::vector<std::vector<SetBlockRelative>> outside_block_to_place_;
    std::unique_ptr<LightStorage> lighting_;
    std::unique_ptr<Heightmap> heightmap_;
    bool is_empty_ = true;

    ChunkContainer();
    virtual ~ChunkContainer();
    ChunkContainer(const ChunkRawData&);
    ChunkContainer(ChunkContainer&&);
    ChunkContainer(const ChunkContainer&) = delete;
    
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
    void SetLightDirty();
private:
    bool light_dirty_ = false;
    Palette block_storage_;
};

#endif
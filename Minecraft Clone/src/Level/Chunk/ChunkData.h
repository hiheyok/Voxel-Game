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
    ChunkContainer();
    virtual ~ChunkContainer();
    ChunkContainer(const ChunkRawData&);
    ChunkContainer(ChunkContainer&&);
    ChunkContainer(const ChunkContainer&) = delete;

    void SetNeighbor(ChunkContainer* neighbor, unsigned int side);
    ChunkContainer* GetNeighbor(unsigned int side) const;
    void ClearNeighbors();

    BlockID GetBlock(const BlockPos& pos) const;
    BlockID GetBlockUnsafe(const BlockPos& pos) const;

    void SetBlock(BlockID block, const BlockPos& pos);
    void SetBlockUnsafe(BlockID block, const BlockPos& pos);

    void SetData(const ChunkRawData& data);
    ChunkRawData GetRawData();
    LightStorage GetLightData();

    void SetPosition(const ChunkPos& pos);

    void UpdateHeightMap();
    void UpdateHeightMap(int x, int z);

    // Check light update request

    bool CheckLightDirty();
    void SetLightDirty();

    ChunkPos position_;
    std::vector<ChunkContainer*> neighbors_;
    std::vector<std::vector<SetBlockRelative>> outside_block_to_place_;
    std::unique_ptr<LightStorage> lighting_;
    std::unique_ptr<Heightmap> heightmap_;
    bool is_empty_ = true;

   private:
    bool light_dirty_ = false;
    Palette block_storage_;
};

#endif
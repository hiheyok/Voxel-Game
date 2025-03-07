#pragma once

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <glm/vec3.hpp>

#include "../DataContainer/BlockData.h"
#include "../TerrainGeneration/Structures/Structure.h"
#include "Lighting/ChunkLighting.h"
#include "../Typenames.h"
#include "ChunkPos/ChunkPos.h"



class ChunkContainer {
    
public:

    ChunkContainer() {
        lighting_ = std::make_unique<ChunkLightingContainer>();
    }
    
    void SetNeighbor(ChunkContainer* neighbor, unsigned int side);

    void ClearNeighbors();

    BlockID GetBlock(int x, int y, int z) const;
    BlockID GetBlockUnsafe(int x, int y, int z) const;

    void SetBlock(BlockID block, int x, int y, int z);
    void SetBlockUnsafe(BlockID block, int x, int y, int z);

    void Use();
    void Unuse();

    ChunkContainer* GetNeighbor(unsigned int side) const;
    
    void SetPosition(int x, int y, int z);

    ChunkPos position_;

    std::atomic<bool> is_empty_ = true;

    ChunkContainer* neighbors_[6]{nullptr};
    std::vector<SetBlockRelative> outside_block_to_place_[6]{};
    std::unique_ptr<ChunkLightingContainer> lighting_;

    BlockContainer block_storage_;
private:
    std::atomic<bool> in_use_ = false;
    
};

#endif
#pragma once

#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <glm/vec3.hpp>
#include "../TerrainGeneration/Structures/Structure.h"
#include "Lighting/ChunkLighting.h"
#include "../Typenames.h"
#include "ChunkPos/ChunkPos.h"
#include "Palette.h"



class ChunkContainer {
private:
    std::atomic<bool> in_use_ = false;

    Palette block_storage_;
public:
    ChunkPos position_;
    std::vector<SetBlockRelative> outside_block_to_place_[6]{};
    ChunkContainer* neighbors_[6]{ nullptr };
    std::unique_ptr<ChunkLightingContainer> lighting_;
    std::atomic<bool> is_empty_ = true;

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
};

#endif
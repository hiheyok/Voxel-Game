#pragma once
#include "ChunkData.h"

enum TerrainType {
    SUPERFLAT, FASTTERRAIN, MOUNTAINS, DEBUG_WORLD, MATH_WORLD, DEBUG_WORLD_2
};

class Chunk : public ChunkContainer {
public:
    Chunk();
    ~Chunk();
    void UpdateGen();
};
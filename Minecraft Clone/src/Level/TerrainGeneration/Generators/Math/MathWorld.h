#pragma once
#include "../Generator.h"

class MathWorld : public WorldGenerator {
public:
    MathWorld() {

    }

    Chunk* Generate(const ChunkPos& pos) override;
};
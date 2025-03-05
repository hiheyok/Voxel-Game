#pragma once
#include "../Generator.h"

class DebugWorldSecond : public WorldGenerator {
public:
    DebugWorldSecond() {

    }

    Chunk* Generate(const ChunkPos& pos) override;
};
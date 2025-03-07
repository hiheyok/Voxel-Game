#pragma once
#include "../Generator.h"

class DebugWorldSecond : public WorldGenerator {
public:
    DebugWorldSecond() {

    }

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};
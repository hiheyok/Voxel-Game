#pragma once
#include "../Generator.h"

class DebugPositioning : public WorldGenerator {
public:
    DebugPositioning() {

    }

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};
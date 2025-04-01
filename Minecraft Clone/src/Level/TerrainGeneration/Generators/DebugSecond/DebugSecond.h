#pragma once
#include "Level/TerrainGeneration/Generators/Generator.h"

class DebugWorldSecond : public WorldGenerator {
public:
    DebugWorldSecond();

    void Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) override;
};
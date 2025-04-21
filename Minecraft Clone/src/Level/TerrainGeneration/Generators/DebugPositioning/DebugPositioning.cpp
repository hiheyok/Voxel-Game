#include "Level/TerrainGeneration/Generators/DebugPositioning/DebugPositioning.h"

DebugPositioning::DebugPositioning() = default;

void DebugPositioning::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    // Red -> x
    ChunkPos newPos = chunk->position_;
    for (int i = 0; i < kChunkDim; ++i) {
        if (newPos.x & 1)
            chunk->SetBlockUnsafe(g_blocks.RED_WOOL, i, 0, 0);
        if (newPos.y & 1)
            chunk->SetBlockUnsafe(g_blocks.GREEN_WOOL, i, 1, 0);
        if (newPos.z & 1)
            chunk->SetBlockUnsafe(g_blocks.BLUE_WOOL, i, 2, 0);

        newPos.x >>= 1;
        newPos.y >>= 1;
        newPos.z >>= 1;
    }

    chunk->SetBlockUnsafe(g_blocks.RED_WOOL, 1, 4, 0);
    chunk->SetBlockUnsafe(g_blocks.GREEN_WOOL, 0, 5, 0);
    chunk->SetBlockUnsafe(g_blocks.BLUE_WOOL, 0, 4, 1);

    chunk->SetBlockUnsafe(g_blocks.GOLD_BLOCK, 0, 3, 0);
    chunk->SetBlockUnsafe(g_blocks.EMERALD_BLOCK, 15, 3, 0);

    for (int i = 1; i < kChunkDim - 1; ++i) {
        if (i & 1) {
            chunk->SetBlockUnsafe(g_blocks.BLACK_CONCRETE, i, 3, 0);
        }
        else {
            chunk->SetBlockUnsafe(g_blocks.WHITE_CONCRETE, i, 3, 0);
        }
    }
}
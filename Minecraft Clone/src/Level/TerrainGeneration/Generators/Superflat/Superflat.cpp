#include "Level/TerrainGeneration/Generators/Superflat/Superflat.h"

SuperflatWorld::SuperflatWorld() = default;

void SuperflatWorld::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    int cy = pos.y * kChunkDim;

    for (int x = 0; x < kChunkDim; x++) {
        for (int z = 0; z < kChunkDim; z++) {
            for (int y = 0; y < kChunkDim; y++) {
                if (y + cy < 10) {
                    chunk->SetBlockUnsafe(g_blocks.WHITE_CONCRETE, x, y, z);
                }
            }
        }
    }
}
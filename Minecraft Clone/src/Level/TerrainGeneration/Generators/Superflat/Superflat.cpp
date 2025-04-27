#include "Level/TerrainGeneration/Generators/Superflat/Superflat.h"

SuperflatWorld::SuperflatWorld() = default;

void SuperflatWorld::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    int cy = pos.y * kChunkDim;
    BlockPos block_pos{0, 0, 0};
    for (block_pos.x = 0; block_pos.x < kChunkDim; block_pos.x++) {
        for (block_pos.z = 0; block_pos.z < kChunkDim; block_pos.z++) {
            for (block_pos.y = 0; block_pos.y < kChunkDim; block_pos.y++) {
                if (block_pos.y + cy < 10) {
                    chunk->SetBlockUnsafe(g_blocks.WHITE_CONCRETE, block_pos);
                }
            }
        }
    }
}
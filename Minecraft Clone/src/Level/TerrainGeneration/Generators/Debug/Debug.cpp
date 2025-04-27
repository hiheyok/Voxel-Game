#include "Level/TerrainGeneration/Generators/Debug/Debug.h"

DebugWorld::DebugWorld() = default;

void DebugWorld::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    int gx = pos.x * kChunkDim;
    int gz = pos.z * kChunkDim;
    int gy = pos.y * kChunkDim;

    if (pos.y < 0) {
        return;
    }

    int numBlocks = static_cast<int>(g_blocks.block_type_data_.size());

    int RowLen = 50;
    int ColLen = 50;

    if (gy == 0) {
        for (int x = 0; x < kChunkDim; x++) {
            for (int z = 0; z < kChunkDim; z++) {

                chunk->SetBlockUnsafe(g_blocks.WHITE_CONCRETE, BlockPos{x, 0, z});
            }
        }
    }
    

    for (int x = 0; x < kChunkDim; x++) {
        for (int z = 0; z < kChunkDim; z++) {
            for (int y = 0; y < kChunkDim; y++) {
                int px = x + gx;
                int pz = z + gz;
                int py = y + gy - 1;

                if (((px & 0b1) == 1) || ((pz & 0b1) == 1) || ((py & 0b1) == 1)) {
                    continue;
                }

                px = px / 2;
                pz = pz / 2;
                py = py / 2;

                if ((px < 0) || (px >= RowLen) || (pz < 0) || (pz >= ColLen)) {
                    continue;
                }

                int b = px + pz * RowLen + py * RowLen * ColLen;

                if ((b < numBlocks) && (b >= 0)) {
                    chunk->SetBlockUnsafe(b, BlockPos{x, y, z});
                }
            }
        }
    }
}
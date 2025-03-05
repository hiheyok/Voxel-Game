#include "Superflat.h"

Chunk* SuperflatWorld::Generate(const ChunkPos& pos) {
    Chunk* chunk = new Chunk;
    int cy = pos.y * 16;

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            for (int y = 0; y < 16; y++) {
                if (y + cy < 10) {
                    chunk->SetBlockUnsafe(g_blocks.WHITE_CONCRETE, x, y, z);
                }
            }
        }
    }
    return chunk;
}
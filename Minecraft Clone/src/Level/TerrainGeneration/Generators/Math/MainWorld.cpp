#include "MathWorld.h"

Chunk* MathWorld::Generate(const ChunkPos& pos) {
    Chunk* chunk = new Chunk;

    int cx = pos.x * 16;
    int cy = pos.y * 16;
    int cz = pos.z * 16;
    float zoom = 20.f;

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 16; y++) {
            for (int z = 0; z < 16; z++) {

                if ((chunk->GetBlock(x, y, z) == g_blocks.RED_STAINED_GLASS) || (chunk->GetBlock(x, y, z) == g_blocks.BLACK_STAINED_GLASS))
                    continue;

                glm::vec3 globalPos(cx + x, cy + y, cz + z);


                float gx = globalPos.x / zoom;
                float gy = globalPos.y / zoom;
                float gz = globalPos.z / zoom;

                //if (0 < gx * gx - gy * gy - gz) {

                //    SetBlock(Blocks.WHITE_CONCRETE, x, y, z);
                //}

                //if (gy > 3 * gz * gz - gx * gx + 2) {
                //    SetBlock(Blocks.WHITE_CONCRETE, x, y, z);
                //}

                if (gz * gz + gy * gy > 4) {
                    chunk->SetBlock(g_blocks.BRICKS, x, y, z);
                }

                int Unit = 1;
                int UnitSize = 5;

                if (gx == 0 && gy == 0) {
                    if (((int)globalPos.z % (Unit * (int)zoom)) == 0) {
                        for (int i = -UnitSize; i <= UnitSize; i++) {
                            for (int j = -UnitSize; j <= UnitSize; j++) {
                                chunk->SetBlock(g_blocks.RED_STAINED_GLASS, x + j, y + i, z);
                            }
                        }
                    }
                    else {
                        chunk->SetBlock(g_blocks.BLACK_STAINED_GLASS, x, y, z);
                    }

                }
                if (gz == 0 && gy == 0) {
                    if (((int)globalPos.x % (Unit * (int)zoom)) == 0) {
                        for (int i = -UnitSize; i <= UnitSize; i++) {
                            for (int j = -UnitSize; j <= UnitSize; j++) {
                                chunk->SetBlock(g_blocks.RED_STAINED_GLASS, x, y + i, z + j);
                            }
                        }
                    }
                    else {
                        chunk->SetBlock(g_blocks.BLACK_STAINED_GLASS, x, y, z);
                    }
                }
                if (gz == 0 && gx == 0) {
                    if (((int)globalPos.y % (Unit * (int)zoom)) == 0) {
                        for (int i = -UnitSize; i <= UnitSize; i++) {
                            for (int j = -UnitSize; j <= UnitSize; j++) {
                                chunk->SetBlock(g_blocks.RED_STAINED_GLASS, x + i, y, z + j);
                            }
                        }
                    }
                    else {
                        chunk->SetBlock(g_blocks.BLACK_STAINED_GLASS, x, y, z);
                    }
                }

            }
        }
    }
    return chunk;
}
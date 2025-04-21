#include "Level/TerrainGeneration/Generators/DebugSecond/DebugSecond.h"
DebugWorldSecond::DebugWorldSecond() = default;

void DebugWorldSecond::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    chunk->SetBlock(g_blocks.COBBLESTONE, 8, 8, 8);

    ChunkPos scaledPos = pos * kChunkDim;

    for (int gx = 0; gx < kChunkDim; gx++) {
        for (int gy = 0; gy < kChunkDim; gy++) {
            for (int gz = 0; gz < kChunkDim; gz++) {

                int index = gz * kChunkSize2D + gy * kChunkDim + gx;


                if (index % 5) {
                    continue;
                }

                int x = gx + scaledPos.x;
                int y = gy + scaledPos.y;
                int z = gz + scaledPos.z;

                if ((abs(x) >= abs(z)) && (abs(x) >= abs(y))) {
                    chunk->SetBlock(g_blocks.BLUE_STAINED_GLASS, gx, gy, gz);
                }
                if ((abs(z) >= abs(x)) && (abs(z) >= abs(y))) {
                    chunk->SetBlock(g_blocks.DARK_OAK_PLANKS, gx, gy, gz);
                }
                if ((abs(y) >= abs(x)) && (abs(y) >= abs(z))) {
                    chunk->SetBlock(g_blocks.ORANGE_CONCRETE, gx, gy, gz);
                }
            }
        }
    }
}
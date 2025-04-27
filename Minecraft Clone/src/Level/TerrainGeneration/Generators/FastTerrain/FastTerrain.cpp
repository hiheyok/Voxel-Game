#include "Level/TerrainGeneration/Generators/FastTerrain/FastTerrain.h"
#include "Utils/FastNoiseLite.h"

FastTerrain::FastTerrain() :noise_{std::make_unique<FastNoiseLite>()} {
    noise_->SetNoiseType(noise_->NoiseType_OpenSimplex2);
    noise_->SetFrequency(0.009f);
}

void FastTerrain::Generate(const ChunkPos& pos, std::unique_ptr<Chunk>& chunk) {
    int cx = pos.x * kChunkDim;
    int cy = pos.y * kChunkDim;
    int cz = pos.z * kChunkDim;

    long long SEED = WorldGenerator::world_seed_;

    for (int x = 0 + cx; x < kChunkDim + cx; x++) {
        for (int z = 0 + cz; z < kChunkDim + cz; z++) {

            float BIOME_MAP = (float)(noise_->GetNoise((float)x / 600, (float)z / 600, (float)SEED) + 1) / 2;
            float TREE_MAP = (float)((double)(noise_->GetNoise((float)x * 10, (float)z * 10, (float)SEED) + 1) / 2) * (BIOME_MAP * 2);
            float MIN_BIOME_MAP = (float)((noise_->GetNoise((float)x / 60, (float)z / 60, (float)SEED) + 1)) * 2;
            float TEMP_MAP = (float)(noise_->GetNoise((float)x / 1000, (float)z / 1000, (float)SEED) + 1) * 2;
            float HEIGHT_MAP = (float)(((noise_->GetNoise((float)x, (float)z, (float)SEED) + 1) * 3) * (((noise_->GetNoise((float)x * 5, (float)z * 5, (float)SEED) + 1) * .25) * BIOME_MAP)) * 30;
            int a = (int)pow(((BIOME_MAP * HEIGHT_MAP + 5) * TEMP_MAP - 10) / (2 * MIN_BIOME_MAP), 1.6) + 5;
            for (int y = 0 + cy; y < kChunkDim + cy; y++) {
                if ((a == y || a + 1 == y || a + 2 == y || a + 3 == y || a + 4 == y) && a > 10) {
                    if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
                        chunk->SetBlock(g_blocks.OAK_LOG, BlockPos{x - cx, y - cy, z - cz});

                    }
                }
                if (a + 5 == y && a > 10) {
                    if (TREE_MAP >= (TREE_RAND_VAL - TREE_RAND_VAL_RANGE) && TREE_MAP <= (TREE_RAND_VAL + TREE_RAND_VAL_RANGE)) {
                        for (int tx = -2; tx <= 2; tx++) {
                            for (int ty = -2; ty <= 2; ty++) {
                                for (int tz = -2; tz <= 2; tz++) {
                                    chunk->SetBlock(g_blocks.OAK_LEAF, BlockPos{x + tx - cx, y + ty - cy, z + tz - cz});
                                }
                            }
                        }

                    }
                }

                if (y < 10) {
                    chunk->SetBlockUnsafe(g_blocks.WATER, BlockPos{x - cx, y - cy, z - cz});
                }

                if (a > y) {
                    if (y < 12) {
                        chunk->SetBlockUnsafe(g_blocks.SAND, BlockPos{x - cx, y - cy, z - cz});
                    }
                    else {

                        if (a >= y) {
                            chunk->SetBlockUnsafe(g_blocks.GRASS, BlockPos{x - cx, y - cy, z - cz});
                            chunk->is_empty_ = false;
                        }
                        if (a - 1 > y) {
                            chunk->SetBlockUnsafe(g_blocks.DIRT, BlockPos{x - cx, y - cy, z - cz});
                        }



                    }

                }


            }

        }
    }
}
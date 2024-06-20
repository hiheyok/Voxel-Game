#pragma once
#include "../Biome.h"

class BiomeSwamp : public Biome {
public:
	BiomeSwamp(BiomeProperties properties) : Biome(properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}

    void genTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) override
    {
        double d0 = GRASS_COLOR_NOISE.getValue((double)x * 0.25, (double)z * 0.25);

        if (d0 > 0.0)
        {
            int i = x & 15;
            int j = z & 15;

            for (int k = 255; k >= 0; --k)
            {
                if (GetBlockChunkSafe(chunk, j, k, i) != Blocks.AIR)
                {
                    if (k == 62 && GetBlockChunkSafe(chunk, j, k, i) != Blocks.WATER)
                    {
                        SetBlockChunkSafe(chunk, Blocks.WATER, j, k, i);

                        //if (d0 < 0.12)
                        //{
                        //    chunk.setBlockState(j, k + 1, i, WATER_LILY);
                        //}
                    }

                    break;
                }
            }
        }

        generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings);
    }
};
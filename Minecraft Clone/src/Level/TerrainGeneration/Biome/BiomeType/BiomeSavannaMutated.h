#pragma once
#include "../Biome.h"

class BiomeSavannaMutated : public BiomeSavanna {
public:
	BiomeSavannaMutated(BiomeProperties properties) : BiomeSavanna(properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}

    void genTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) override
    {
        topBlock = Blocks.GRASS;
        fillerBlock = Blocks.DIRT;

        if (noiseVal > 1.75)
        {
            topBlock = Blocks.STONE;
            fillerBlock = Blocks.STONE;
        }
        else if (noiseVal > -0.5)
        {
            topBlock = Blocks.COARSE_DIRT;
        }

        generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings);
    }
};
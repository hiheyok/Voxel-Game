#pragma once
#include "../Biome.h"

class BiomeTaiga : public Biome {
public:
    enum Type {
        NORMAL,
        MEGA,
        MEGA_SPRUCE
    };

    BiomeTaiga::Type type;

    BiomeTaiga(BiomeTaiga::Type biomeType, BiomeProperties properties) : Biome(properties), type(biomeType) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }

    void genTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) override {
        if (type == BiomeTaiga::Type::MEGA || type == BiomeTaiga::Type::MEGA_SPRUCE)
        {
            topBlock = Blocks.GRASS;
            fillerBlock = Blocks.DIRT;

            if (noiseVal > 1.75)
            {
                topBlock = Blocks.COARSE_DIRT;
            }
            else if (noiseVal > -0.95)
            {
                topBlock = Blocks.PODZOL;
            }
        }

        generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings);
    }

};
#pragma once
#include "../Biome.h"

class BiomeTaiga : public Biome {
public:
    enum Type {
        NORMAL,
        MEGA,
        MEGA_SPRUCE
    };

    BiomeTaiga::Type type_;

    BiomeTaiga(BiomeTaiga::Type biomeType, BiomeProperties properties) : Biome(properties), type_(biomeType) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }

    void GenTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings_) override {
        if (type_ == BiomeTaiga::Type::MEGA || type_ == BiomeTaiga::Type::MEGA_SPRUCE)
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

        generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings_);
    }

};
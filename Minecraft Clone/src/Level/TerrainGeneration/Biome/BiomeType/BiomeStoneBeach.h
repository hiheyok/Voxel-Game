#pragma once
#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeStoneBeach : public Biome {
   public:
    BiomeStoneBeach(BiomeProperties properties) : Biome(properties) {
        topBlock = g_blocks.STONE;
        fillerBlock = g_blocks.STONE;
    }

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
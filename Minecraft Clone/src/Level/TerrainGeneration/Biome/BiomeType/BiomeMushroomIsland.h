#pragma once
#include "../Biome.h"

class BiomeMushroomIsland : public Biome {
public:
    BiomeMushroomIsland(BiomeProperties properties) : Biome(properties) {
        topBlock = g_blocks.MYCELIUM;
    }

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }

    
};
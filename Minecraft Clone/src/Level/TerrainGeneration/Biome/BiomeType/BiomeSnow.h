#pragma once
#include "../Biome.h"

class BiomeSnow : public Biome {
public:
    bool superIcy = false;

    BiomeSnow(bool superIcyIn, BiomeProperties properties) : Biome(properties), superIcy(superIcyIn) {
        if (superIcyIn) {
            topBlock = g_blocks.SNOW_BLOCK;
        }
    }

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
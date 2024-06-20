#pragma once
#include "../Biome.h"

class BiomeDesert : public Biome {
public:
    BiomeDesert(BiomeProperties properties) : Biome(properties) {
        topBlock = Blocks.SAND;
        topBlock = Blocks.SAND;
    }

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
#pragma once
#include "../Biome.h"

class BiomeForest : public Biome {
public:
    enum Type {
        NORMAL,
        FLOWER,
        BIRCH,
        ROOFED
    };

    BiomeForest::Type type;

    BiomeForest(BiomeForest::Type biomeType, BiomeProperties properties) : Biome(properties), type(biomeType) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
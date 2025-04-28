#pragma once
#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeForest : public Biome {
   public:
    enum Type { NORMAL, FLOWER, BIRCH, ROOFED };

    BiomeForest::Type type_;

    BiomeForest(BiomeForest::Type biomeType, BiomeProperties properties)
        : Biome(properties), type_(biomeType) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
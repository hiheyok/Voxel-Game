#pragma once
#include "../Biome.h"

class BiomePlains : public Biome {
public:
    bool sunglowers = false;

    BiomePlains(bool sunflowersIn, BiomeProperties p) : Biome(p), sunglowers(sunflowersIn) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
#pragma once
#include "../Biome.h"

class BiomeRiver : public Biome {
public:
    BiomeRiver(BiomeProperties properties) : Biome(properties) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
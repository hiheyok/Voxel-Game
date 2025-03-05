#pragma once
#include "../Biome.h"

class BiomeJungle : public Biome {
public:
    bool is_edge_ = false;

    BiomeJungle(bool isEdgeIn, BiomeProperties properties) : Biome(properties), is_edge_(isEdgeIn) {}

    const std::type_info& getBiomeClass() const override {
        return typeid(*this);
    }
};
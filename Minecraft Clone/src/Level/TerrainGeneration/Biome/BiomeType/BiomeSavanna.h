#pragma once
#include "../Biome.h"

class BiomeSavanna : public Biome {
public:
	BiomeSavanna(BiomeProperties properties) : Biome(properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
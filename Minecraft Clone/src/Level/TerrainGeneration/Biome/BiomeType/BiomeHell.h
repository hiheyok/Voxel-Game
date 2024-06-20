#pragma once
#include "../Biome.h"

class BiomeHell : public Biome {
public:
	BiomeHell(BiomeProperties properties) : Biome(properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
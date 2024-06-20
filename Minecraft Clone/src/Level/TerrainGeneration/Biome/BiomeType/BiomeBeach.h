#pragma once
#include "../Biome.h"

class BiomeBeach : public Biome {
public:
	BiomeBeach(BiomeProperties properties) : Biome(properties) {
		topBlock = Blocks.SAND;
		fillerBlock = Blocks.SAND;
	}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
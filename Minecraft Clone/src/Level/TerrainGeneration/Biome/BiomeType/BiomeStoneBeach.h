#pragma once
#include "../Biome.h"

class BiomeStoneBeach : public Biome {
public:
	BiomeStoneBeach(BiomeProperties properties) : Biome(properties) {
		topBlock = Blocks.STONE;
		fillerBlock = Blocks.STONE;
	}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
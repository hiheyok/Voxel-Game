#pragma once
#include "../Biome.h"

class BiomeEnd : public Biome {
public:
	BiomeEnd(BiomeProperties properties) : Biome(properties) {
		topBlock = Blocks.DIRT;
		fillerBlock = Blocks.DIRT;
	}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
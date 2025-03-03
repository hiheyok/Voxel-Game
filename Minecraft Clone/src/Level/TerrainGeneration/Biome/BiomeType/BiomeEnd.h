#pragma once
#include "../Biome.h"

class BiomeEnd : public Biome {
public:
	BiomeEnd(BiomeProperties properties) : Biome(properties) {
		topBlock = g_blocks.DIRT;
		fillerBlock = g_blocks.DIRT;
	}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
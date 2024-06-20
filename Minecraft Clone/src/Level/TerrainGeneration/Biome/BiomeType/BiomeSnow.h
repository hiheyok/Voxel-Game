#pragma once
#include "../Biome.h"

class BiomeSnow : public Biome {
public:
	bool superIcy = false;

	BiomeSnow(bool superIcyIn, BiomeProperties properties) : superIcy(superIcyIn), Biome(properties) {
		if (superIcyIn) {
			topBlock = Blocks.SNOW_BLOCK;
		}
	}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
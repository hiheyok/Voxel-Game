#pragma once
#include "../Biome.h"

class BiomePlains : public Biome {
public:
	bool sunglowers = false;

	BiomePlains(bool sunflowersIn, BiomeProperties p) : sunglowers(sunflowersIn), Biome(p) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
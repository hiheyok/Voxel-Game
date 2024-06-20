#pragma once
#include "../Biome.h"
#include "BiomeForest.h"

class BiomeForestMutated : public BiomeForest {
public:
	BiomeForestMutated(BiomeProperties properties) : BiomeForest(BiomeForest::Type::BIRCH, properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
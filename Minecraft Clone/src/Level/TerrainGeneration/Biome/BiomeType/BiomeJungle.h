#pragma once
#include "../Biome.h"

class BiomeJungle : public Biome {
public:
	bool isEdge = false;

	BiomeJungle(bool isEdgeIn, BiomeProperties properties) : isEdge(isEdgeIn), Biome(properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}
};
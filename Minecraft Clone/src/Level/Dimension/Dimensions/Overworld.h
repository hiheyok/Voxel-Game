#pragma once
#include "../Dimension.h"

class OverworldDimension : public Dimension {
public:
	OverworldDimension() : Dimension(Generators.MOUNTAINS) {}

	void Tick() override {

	}
};
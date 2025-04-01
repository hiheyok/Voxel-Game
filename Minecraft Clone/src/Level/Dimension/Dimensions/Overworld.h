#pragma once
#include "../Dimension.h"

class OverworldDimension : public Dimension {
public:
    OverworldDimension() : Dimension(g_generators.MINECRAFT) {}

    void Tick() override {

    }
};
#pragma once
#include "Level/Dimension/Dimension.h"

class OverworldDimension : public Dimension {
public:
    OverworldDimension() : Dimension(g_generators.MINECRAFT) {}

    void Tick() override {

    }
};
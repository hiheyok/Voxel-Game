#pragma once
#include <vector>
#include "Generators.h"
#include "../../Typenames.h"

class WorldGenerator;

class GeneratorType {
private:

    std::vector<WorldGenerator*> generator_list_ = {};

    WorldGeneratorID RegisterWorldGenerator(WorldGenerator* Gen);

public:
    WorldGeneratorID DEBUG = RegisterWorldGenerator(new DebugWorld());
    WorldGeneratorID DEBUG_2 = RegisterWorldGenerator(new DebugWorldSecond());
    WorldGeneratorID DEBUG_POSITIONING = RegisterWorldGenerator(new DebugPositioning());
    WorldGeneratorID FAST_TERRAIN = RegisterWorldGenerator(new FastTerrain());
    WorldGeneratorID MATH_WORLD = RegisterWorldGenerator(new MathWorld());
    WorldGeneratorID MOUNTAINS = RegisterWorldGenerator(new MountainGenerator());
    WorldGeneratorID SUPERFLAT = RegisterWorldGenerator(new SuperflatWorld());
    WorldGeneratorID MINECRAFT = RegisterWorldGenerator(new MinecraftTerrain());

    WorldGenerator* GetGenerator(WorldGeneratorID ID);
};

extern GeneratorType g_generators;
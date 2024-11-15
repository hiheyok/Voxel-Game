#pragma once
#include "Generators.h"
#include <vector>
#include "../../../Utils/LogUtils.h"

typedef uint64_t WorldGeneratorID;

class GeneratorType {
private:

	std::vector<WorldGenerator*> GeneratorList = {};

	WorldGeneratorID RegisterWorldGenerator(WorldGenerator* Gen);

public:
	WorldGeneratorID DEBUG = RegisterWorldGenerator(new DebugWorld());
	WorldGeneratorID DEBUG_2 = RegisterWorldGenerator(new DebugWorldSecond());
	WorldGeneratorID FAST_TERRAIN = RegisterWorldGenerator(new FastTerrain());
	WorldGeneratorID MATH_WORLD = RegisterWorldGenerator(new MathWorld());
	WorldGeneratorID MOUNTAINS = RegisterWorldGenerator(new MountainGenerator());
	WorldGeneratorID SUPERFLAT = RegisterWorldGenerator(new SuperflatWorld());
	WorldGeneratorID MINECRAFT = RegisterWorldGenerator(new MinecraftTerrain());

	WorldGenerator* GetGenerator(WorldGeneratorID ID);
};

extern GeneratorType Generators;
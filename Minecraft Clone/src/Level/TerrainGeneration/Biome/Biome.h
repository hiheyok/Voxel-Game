#pragma once
#include <unordered_map>
#include "../../../Core/Registry/Registry.h"
#include "../../../Core/Registry/ResourceLocation.h"
#include "BiomeProperties.h"
class Biome {
private:

public:
	BiomeProperties biomeProperties;

	static Registry<ResourceLocation, Biome*> REGISTRY;

	static void Register(int ID, std::string BiomeName, Biome* biome) {
		REGISTRY.Register(ID, ResourceLocation(BiomeName), biome);
	}

	static void registerBiome() {

	}

};
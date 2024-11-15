#pragma once
#include "../Biome.h"

class BiomeHills : public Biome {
public:
	
	enum Type {
		NORMAL,
		EXTRA_TREES,
		MUTATED
	};

	BiomeHills::Type type;

	BiomeHills(BiomeHills::Type biomeType, BiomeProperties properties) : type(biomeType), Biome(properties) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}

	void genTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) override {
		topBlock = Blocks.GRASS;
		fillerBlock = Blocks.DIRT;

		if ((noiseVal < -1.0 || noiseVal > 2.0) && type == BiomeHills::Type::MUTATED)
		{
			topBlock = Blocks.GRAVEL;
			fillerBlock = Blocks.GRAVEL;
		}
		else if (noiseVal > 1.0 && type != BiomeHills::Type::EXTRA_TREES)
		{
			topBlock = Blocks.STONE;
			fillerBlock = Blocks.STONE;
		}

		generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings);
	}

	
};
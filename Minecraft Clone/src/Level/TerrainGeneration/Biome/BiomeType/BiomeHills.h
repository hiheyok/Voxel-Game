#pragma once
#include "../Biome.h"

class BiomeHills : public Biome {
public:
	
	enum Type {
		NORMAL,
		EXTRA_TREES,
		MUTATED
	};

	BiomeHills::Type type_;

	BiomeHills(BiomeHills::Type biomeType, BiomeProperties properties) : Biome(properties), type_(biomeType) {}

	const std::type_info& getBiomeClass() const override {
		return typeid(*this);
	}

	void GenTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings_) override {
		topBlock = Blocks.GRASS;
		fillerBlock = Blocks.DIRT;

		if ((noiseVal < -1.0 || noiseVal > 2.0) && type_ == BiomeHills::Type::MUTATED)
		{
			topBlock = Blocks.GRAVEL;
			fillerBlock = Blocks.GRAVEL;
		}
		else if (noiseVal > 1.0 && type_ != BiomeHills::Type::EXTRA_TREES)
		{
			topBlock = Blocks.STONE;
			fillerBlock = Blocks.STONE;
		}

		generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings_);
	}

	
};
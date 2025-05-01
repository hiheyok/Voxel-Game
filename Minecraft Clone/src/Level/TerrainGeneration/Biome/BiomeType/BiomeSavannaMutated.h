#pragma once
#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeSavannaMutated : public BiomeSavanna {
 public:
  BiomeSavannaMutated(BiomeProperties properties) : BiomeSavanna(properties) {}

  const std::type_info& getBiomeClass() const override { return typeid(*this); }

  void GenTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z,
                        double noiseVal,
                        ChunkGeneratorSettings* settings_) override {
    topBlock = g_blocks.GRASS;
    fillerBlock = g_blocks.DIRT;

    if (noiseVal > 1.75) {
      topBlock = g_blocks.STONE;
      fillerBlock = g_blocks.STONE;
    } else if (noiseVal > -0.5) {
      topBlock = g_blocks.COARSE_DIRT;
    }

    generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings_);
  }
};

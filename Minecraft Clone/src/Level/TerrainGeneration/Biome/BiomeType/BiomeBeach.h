#pragma once
#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeBeach : public Biome {
 public:
  BiomeBeach(BiomeProperties properties) : Biome(properties) {
    topBlock = g_blocks.SAND;
    fillerBlock = g_blocks.SAND;
  }

  const std::type_info& getBiomeClass() const override { return typeid(*this); }
};

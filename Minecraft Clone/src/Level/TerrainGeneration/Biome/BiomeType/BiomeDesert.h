#pragma once
#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeDesert : public Biome {
 public:
  BiomeDesert(BiomeProperties properties) : Biome(properties) {
    topBlock = g_blocks.SAND;
    topBlock = g_blocks.SAND;
  }

  const std::type_info& getBiomeClass() const override { return typeid(*this); }
};

#pragma once
#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeVoid : public Biome {
 public:
  BiomeVoid(BiomeProperties properties) : Biome(properties) {}

  const std::type_info& getBiomeClass() const override { return typeid(*this); }
};

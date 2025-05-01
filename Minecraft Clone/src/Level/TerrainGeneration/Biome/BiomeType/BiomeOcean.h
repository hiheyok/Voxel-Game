#pragma once

#include "Level/TerrainGeneration/Biome/Biome.h"

class BiomeOcean : public Biome {
 public:
  BiomeOcean(BiomeProperties p) : Biome(p) {}

  const std::type_info& getBiomeClass() const override { return typeid(*this); }

  Biome::TempCategory getTempCategory() override {
    return Biome::TempCategory::OCEAN;
  }
};

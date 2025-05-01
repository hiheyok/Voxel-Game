#pragma once
#include "Level/TerrainGeneration/Biome/Biomes.h"
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerHills : public GenLayer {
 private:
  GenLayer* riverLayer = nullptr;

 public:
  GenLayerHills(int64_t baseSeedIn, GenLayer* parentIn, GenLayer* riverLayerIn)
      : GenLayer(baseSeedIn) {
    parent = parentIn;
    riverLayer = riverLayerIn;
  }

  std::vector<int> getInts(int areaX, int areaY, int areaWidth,
                           int areaHeight) {
    std::vector<int> aint =
        parent->getInts(areaX - 1, areaY - 1, areaWidth + 2, areaHeight + 2);
    std::vector<int> aint1 = riverLayer->getInts(areaX - 1, areaY - 1,
                                                 areaWidth + 2, areaHeight + 2);
    std::vector<int> aint2 = std::vector<int>(areaWidth * areaHeight);

    for (int i = 0; i < areaHeight; ++i) {
      for (int j = 0; j < areaWidth; ++j) {
        initChunkSeed(static_cast<int64_t>(j + areaX),
                      static_cast<int64_t>(i + areaY));
        int k = aint[j + 1 + (i + 1) * (areaWidth + 2)];
        int l = aint1[j + 1 + (i + 1) * (areaWidth + 2)];
        bool flag = (l - 2) % 29 == 0;

        if (k > 255) {
          // LOGGER.debug("old! {}", Integer.valueOf(k));
        }

        Biome* biome = Biome::getBiomeForId(k);
        bool flag1 = biome != nullptr && biome->isMutation();

        if (k != 0 && l >= 2 && (l - 2) % 29 == 1 && !flag1) {
          Biome* biome3 = Biome::getMutationForBiome(biome);
          aint2[j + i * areaWidth] =
              biome3 == nullptr ? k : Biome::getIdForBiome(biome3);
        } else if (NextInt(3) != 0 && !flag) {
          aint2[j + i * areaWidth] = k;
        } else {
          Biome* biome1 = biome;

          if (biome == Biomes::DESERT) {
            biome1 = Biomes::DESERT_HILLS;
          } else if (biome == Biomes::FOREST) {
            biome1 = Biomes::FOREST_HILLS;
          } else if (biome == Biomes::BIRCH_FOREST) {
            biome1 = Biomes::BIRCH_FOREST_HILLS;
          } else if (biome == Biomes::ROOFED_FOREST) {
            biome1 = Biomes::PLAINS;
          } else if (biome == Biomes::TAIGA) {
            biome1 = Biomes::TAIGA_HILLS;
          } else if (biome == Biomes::REDWOOD_TAIGA) {
            biome1 = Biomes::REDWOOD_TAIGA_HILLS;
          } else if (biome == Biomes::COLD_TAIGA) {
            biome1 = Biomes::COLD_TAIGA_HILLS;
          } else if (biome == Biomes::PLAINS) {
            if (NextInt(3) == 0) {
              biome1 = Biomes::FOREST_HILLS;
            } else {
              biome1 = Biomes::FOREST;
            }
          } else if (biome == Biomes::ICE_PLAINS) {
            biome1 = Biomes::ICE_MOUNTAINS;
          } else if (biome == Biomes::JUNGLE) {
            biome1 = Biomes::JUNGLE_HILLS;
          } else if (biome == Biomes::OCEAN) {
            biome1 = Biomes::DEEP_OCEAN;
          } else if (biome == Biomes::EXTREME_HILLS) {
            biome1 = Biomes::EXTREME_HILLS_WITH_TREES;
          } else if (biome == Biomes::SAVANNA) {
            biome1 = Biomes::SAVANNA_PLATEAU;
          } else if (biomesEqualOrMesaPlateau(
                         k, Biome::getIdForBiome(Biomes::MESA_ROCK))) {
            biome1 = Biomes::MESA;
          } else if (biome == Biomes::DEEP_OCEAN && NextInt(3) == 0) {
            int i1 = NextInt(2);

            if (i1 == 0) {
              biome1 = Biomes::PLAINS;
            } else {
              biome1 = Biomes::FOREST;
            }
          }

          int j2 = Biome::getIdForBiome(biome1);

          if (flag && j2 != k) {
            Biome* biome2 = Biome::getMutationForBiome(biome1);
            j2 = biome2 == nullptr ? k : Biome::getIdForBiome(biome2);
          }

          if (j2 == k) {
            aint2[j + i * areaWidth] = k;
          } else {
            int k2 = aint[j + 1 + (i + 0) * (areaWidth + 2)];
            int j1 = aint[j + 2 + (i + 1) * (areaWidth + 2)];
            int k1 = aint[j + 0 + (i + 1) * (areaWidth + 2)];
            int l1 = aint[j + 1 + (i + 2) * (areaWidth + 2)];
            int i2 = 0;

            if (biomesEqualOrMesaPlateau(k2, k)) {
              ++i2;
            }

            if (biomesEqualOrMesaPlateau(j1, k)) {
              ++i2;
            }

            if (biomesEqualOrMesaPlateau(k1, k)) {
              ++i2;
            }

            if (biomesEqualOrMesaPlateau(l1, k)) {
              ++i2;
            }

            if (i2 >= 3) {
              aint2[j + i * areaWidth] = j2;
            } else {
              aint2[j + i * areaWidth] = k;
            }
          }
        }
      }
    }

    return aint2;
  }
};

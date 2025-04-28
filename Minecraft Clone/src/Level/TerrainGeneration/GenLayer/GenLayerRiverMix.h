#pragma once
#include "Level/TerrainGeneration/Biome/Biomes.h"
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerRiverMix : public GenLayer {
   private:
    GenLayer* biomePatternGeneratorChain = nullptr;
    GenLayer* riverPatternGeneratorChain = nullptr;

   public:
    GenLayerRiverMix(long long baseSeedIn,
                     GenLayer* biomePatternGeneratorChainIn,
                     GenLayer* riverPatternGeneratorChainIn)
        : GenLayer(baseSeedIn) {
        biomePatternGeneratorChain = biomePatternGeneratorChainIn;
        riverPatternGeneratorChain = riverPatternGeneratorChainIn;
    }

    void initWorldGenSeed(long long seed) override {
        biomePatternGeneratorChain->initWorldGenSeed(seed);
        riverPatternGeneratorChain->initWorldGenSeed(seed);
        GenLayer::initWorldGenSeed(seed);  //
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth,
                             int areaHeight) override {
        std::vector<int> aint = biomePatternGeneratorChain->getInts(
            areaX, areaY, areaWidth, areaHeight);
        std::vector<int> aint1 = riverPatternGeneratorChain->getInts(
            areaX, areaY, areaWidth, areaHeight);
        std::vector<int> aint2 = std::vector<int>(areaWidth * areaHeight);

        for (int i = 0; i < areaWidth * areaHeight; ++i) {
            if (aint[i] != Biome::getIdForBiome(Biomes::OCEAN) &&
                aint[i] != Biome::getIdForBiome(Biomes::DEEP_OCEAN)) {
                if (aint1[i] == Biome::getIdForBiome(Biomes::RIVER)) {
                    if (aint[i] == Biome::getIdForBiome(Biomes::ICE_PLAINS)) {
                        aint2[i] = Biome::getIdForBiome(Biomes::FROZEN_RIVER);
                    } else if (aint[i] != Biome::getIdForBiome(
                                              Biomes::MUSHROOM_ISLAND) &&
                               aint[i] != Biome::getIdForBiome(
                                              Biomes::MUSHROOM_ISLAND_SHORE)) {
                        aint2[i] = aint1[i] & 255;
                    } else {
                        aint2[i] =
                            Biome::getIdForBiome(Biomes::MUSHROOM_ISLAND_SHORE);
                    }
                } else {
                    aint2[i] = aint[i];
                }
            } else {
                aint2[i] = aint[i];
            }
        }
        return aint2;
    }
};
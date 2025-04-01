#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerAddMushroomIsland : public GenLayer {
public:
    GenLayerAddMushroomIsland(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
        parent = parentIn;
    }


    std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override {
        int i = areaX - 1;
        int j = areaY - 1;
        int k = areaWidth + 2;
        int l = areaHeight + 2;
        std::vector<int> aint = parent->getInts(i, j, k, l);
        std::vector<int> aint1 = std::vector<int>(areaWidth * areaHeight); //int cache

        for (int i1 = 0; i1 < areaHeight; ++i1)
        {
            for (int j1 = 0; j1 < areaWidth; ++j1)
            {
                int k1 = aint[j1 + 0 + (i1 + 0) * k];
                int l1 = aint[j1 + 2 + (i1 + 0) * k];
                int i2 = aint[j1 + 0 + (i1 + 2) * k];
                int j2 = aint[j1 + 2 + (i1 + 2) * k];
                int k2 = aint[j1 + 1 + (i1 + 1) * k];
                initChunkSeed((long long)(j1 + areaX), (long long)(i1 + areaY));

                if (k2 == 0 && k1 == 0 && l1 == 0 && i2 == 0 && j2 == 0 && NextInt(100) == 0)
                {
                    aint1[j1 + i1 * areaWidth] = Biome::getIdForBiome(Biomes::MUSHROOM_ISLAND);
                }
                else
                {
                    aint1[j1 + i1 * areaWidth] = k2;
                }
            }
        }

        return aint1;
    }
};
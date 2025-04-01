#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerRiverInit : public GenLayer {
public:
    GenLayerRiverInit(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
        parent = parentIn;
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override {
        std::vector<int> aint = parent->getInts(areaX, areaY, areaWidth, areaHeight);
        std::vector<int> aint1 = std::vector<int>(areaWidth * areaHeight); // int cache

        for (int i = 0; i < areaHeight; ++i)
        {
            for (int j = 0; j < areaWidth; ++j)
            {
                initChunkSeed((long long)(j + areaX), (long long)(i + areaY));
                aint1[j + i * areaWidth] = aint[j + i * areaWidth] > 0 ? NextInt(299999) + 2 : 0;
            }
        }

        return aint1;
    }
};
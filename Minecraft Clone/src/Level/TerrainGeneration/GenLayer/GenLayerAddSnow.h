#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerAddSnow : public GenLayer {
   public:
    GenLayerAddSnow(long long baseSeedIn, GenLayer* parentIn)
        : GenLayer(baseSeedIn) {
        parent = parentIn;
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth,
                             int areaHeight) override {
        int i = areaX - 1;
        int j = areaY - 1;
        int k = areaWidth + 2;
        int l = areaHeight + 2;
        std::vector<int> aint = parent->getInts(i, j, k, l);
        std::vector<int> aint1 =
            std::vector<int>(areaWidth * areaHeight);  // int cache

        for (int i1 = 0; i1 < areaHeight; ++i1) {
            for (int j1 = 0; j1 < areaWidth; ++j1) {
                int k1 = aint[j1 + 1 + (i1 + 1) * k];
                initChunkSeed((long long)(j1 + areaX), (long long)(i1 + areaY));

                if (k1 == 0) {
                    aint1[j1 + i1 * areaWidth] = 0;
                } else {
                    int l1 = NextInt(6);

                    if (l1 == 0) {
                        l1 = 4;
                    } else if (l1 <= 1) {
                        l1 = 3;
                    } else {
                        l1 = 1;
                    }

                    aint1[j1 + i1 * areaWidth] = l1;
                }
            }
        }

        return aint1;
    }
};
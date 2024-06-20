#pragma once
#include "GenLayer.h"
#include "IntCache.h"

class GenLayerRemoveTooMuchOcean : public GenLayer {
public:
	GenLayerRemoveTooMuchOcean(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
		parent = parentIn;
	}

    std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override {
        int i = areaX - 1;
        int j = areaY - 1;
        int k = areaWidth + 2;
        int l = areaHeight + 2;
        std::vector<int>  aint = parent->getInts(i, j, k, l);
        std::vector<int>  aint1 = std::vector<int>(areaWidth * areaHeight); //int cache

        for (int i1 = 0; i1 < areaHeight; ++i1)
        {
            for (int j1 = 0; j1 < areaWidth; ++j1)
            {
                int k1 = aint[j1 + 1 + (i1 + 1 - 1) * (areaWidth + 2)];
                int l1 = aint[j1 + 1 + 1 + (i1 + 1) * (areaWidth + 2)];
                int i2 = aint[j1 + 1 - 1 + (i1 + 1) * (areaWidth + 2)];
                int j2 = aint[j1 + 1 + (i1 + 1 + 1) * (areaWidth + 2)];
                int k2 = aint[j1 + 1 + (i1 + 1) * k];
                aint1[j1 + i1 * areaWidth] = k2;
                initChunkSeed((long long)(j1 + areaX), (long long)(i1 + areaY));

                if (k2 == 0 && k1 == 0 && l1 == 0 && i2 == 0 && j2 == 0 && nextInt(2) == 0)
                {
                    aint1[j1 + i1 * areaWidth] = 1;
                }
            }
        }

        return aint1;
    }
};
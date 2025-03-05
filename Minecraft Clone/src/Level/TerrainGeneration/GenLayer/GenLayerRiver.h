#pragma once
#include "GenLayer.h"
#include "IntCache.h"

class GenLayerRiver : public GenLayer {
private:
    int riverFilter(int p_151630_1_)
    {
        return p_151630_1_ >= 2 ? 2 + (p_151630_1_ & 1) : p_151630_1_;
    }
public:
    GenLayerRiver(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
        parent = parentIn;
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override
    {
        int i = areaX - 1;
        int j = areaY - 1;
        int k = areaWidth + 2;
        int l = areaHeight + 2;
        std::vector<int> aint = parent->getInts(i, j, k, l);
        std::vector<int> aint1 = std::vector<int>(areaWidth * areaHeight); // int cache

        for (int i1 = 0; i1 < areaHeight; ++i1)
        {
            for (int j1 = 0; j1 < areaWidth; ++j1)
            {
                int k1 = riverFilter(aint[j1 + 0 + (i1 + 1) * k]);
                int l1 = riverFilter(aint[j1 + 2 + (i1 + 1) * k]);
                int i2 = riverFilter(aint[j1 + 1 + (i1 + 0) * k]);
                int j2 = riverFilter(aint[j1 + 1 + (i1 + 2) * k]);
                int k2 = riverFilter(aint[j1 + 1 + (i1 + 1) * k]);

                if (k2 == k1 && k2 == i2 && k2 == l1 && k2 == j2)
                {
                    aint1[j1 + i1 * areaWidth] = -1;
                }
                else
                {
                    aint1[j1 + i1 * areaWidth] = Biome::getIdForBiome(Biomes::RIVER);
                }
            }
        }

        return aint1;
    }

    
};
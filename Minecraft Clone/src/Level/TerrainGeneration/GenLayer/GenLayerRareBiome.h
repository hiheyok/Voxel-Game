#pragma once
#include "GenLayer.h"
#include "IntCache.h"

class GenLayerRareBiome : public GenLayer {
public:
    GenLayerRareBiome(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
        parent = parentIn;
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override {
        std::vector<int> aint = parent->getInts(areaX - 1, areaY - 1, areaWidth + 2, areaHeight + 2);
        std::vector<int> aint1 = std::vector<int>(areaWidth * areaHeight);

        for (int i = 0; i < areaHeight; ++i)
        {
            for (int j = 0; j < areaWidth; ++j)
            {
                initChunkSeed((long long)(j + areaX), (long long)(i + areaY));
                int k = aint[j + 1 + (i + 1) * (areaWidth + 2)];

                if (NextInt(57) == 0)
                {
                    if (k == Biome::getIdForBiome(Biomes::PLAINS))
                    {
                        aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::MUTATED_PLAINS);
                    }
                    else
                    {
                        aint1[j + i * areaWidth] = k;
                    }
                }
                else
                {
                    aint1[j + i * areaWidth] = k;
                }
            }
        }

        return aint1;
    }
};
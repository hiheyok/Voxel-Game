#pragma once
#include "GenLayer.h"
#include "IntCache.h"
class GenLayerZoom : public GenLayer {
public:
	GenLayerZoom(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
		parent = parentIn;
	}

   std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override {
        int i = areaX >> 1;
        int j = areaY >> 1;
        int k = (areaWidth >> 1) + 2;
        int l = (areaHeight >> 1) + 2;
        std::vector<int> aint = parent->getInts(i, j, k, l);
        int i1 = k - 1 << 1;
        int j1 = l - 1 << 1;
        std::vector<int> aint1 = std::vector<int>(i1 * j1); //int cache

        for (int k1 = 0; k1 < l - 1; ++k1)
        {
            int l1 = (k1 << 1) * i1;
            int i2 = 0;
            int j2 = aint[i2 + 0 + (k1 + 0) * k];

            for (int k2 = aint[i2 + 0 + (k1 + 1) * k]; i2 < k - 1; ++i2)
            {
                initChunkSeed((long long)(i2 + i << 1), (long long)(k1 + j << 1));
                int l2 = aint[i2 + 1 + (k1 + 0) * k];
                int i3 = aint[i2 + 1 + (k1 + 1) * k];
                aint1[l1] = j2;
                aint1[l1++ + i1] = selectRandom({j2, k2});
                aint1[l1] = selectRandom({j2, l2});
                aint1[l1++ + i1] = selectModeOrRandom(j2, l2, k2, i3);
                j2 = l2;
                k2 = i3;
            }
        }

        std::vector<int> aint2 = std::vector<int>(areaWidth * areaHeight); //int cache

        for (int j3 = 0; j3 < areaHeight; ++j3)
        {
            std::copy(
                aint1.begin() + (j3 + (areaY & 1)) * i1 + (areaX & 1),
                aint1.begin() + (j3 + (areaY & 1)) * i1 + (areaX & 1) + areaWidth,
                aint2.begin() + j3 * areaWidth
            );
        }

        return aint2;
    }

    static GenLayer* magnify(long long p_75915_0_, GenLayer* p_75915_2_, int p_75915_3_) {
        GenLayer* genlayer = p_75915_2_;

        for (int i = 0; i < p_75915_3_; ++i)
        {
            genlayer = new GenLayerZoom(p_75915_0_ + (long long)i, genlayer);
        }

        return genlayer;
    }
};
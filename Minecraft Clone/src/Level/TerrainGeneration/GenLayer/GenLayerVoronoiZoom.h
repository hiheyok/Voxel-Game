#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerVoronoiZoom : public GenLayer {
   public:
    GenLayerVoronoiZoom(long long int baseSeedIn, GenLayer* parentIn)
        : GenLayer(baseSeedIn) {
        parent = parentIn;
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth,
                             int areaHeight) override {
        areaX = areaX - 2;
        areaY = areaY - 2;
        int i = areaX >> 2;
        int j = areaY >> 2;
        int k = (areaWidth >> 2) + 2;
        int l = (areaHeight >> 2) + 2;
        std::vector<int> aint = parent->getInts(i, j, k, l);
        int i1 = (k - 1) << 2;
        int j1 = (l - 1) << 2;
        std::vector<int> aint1 =
            std::vector<int>(i1 * j1);  // IntCache.getIntCache(i1 * j1);

        for (int k1 = 0; k1 < l - 1; ++k1) {
            int l1 = 0;
            int i2 = aint[l1 + 0 + (k1 + 0) * k];

            for (int j2 = aint[l1 + 0 + (k1 + 1) * k]; l1 < k - 1; ++l1) {
                // double d0 = 3.6;
                initChunkSeed(static_cast<long long>(l1 + i) << 2,
                              static_cast<long long>(k1 + j) << 2);
                double d1 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6;
                double d2 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6;
                initChunkSeed(static_cast<long long>(l1 + i + 1) << 2,
                              static_cast<long long>(k1 + j) << 2);
                double d3 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6 + 4.0;
                double d4 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6;
                initChunkSeed(static_cast<long long>(l1 + i) << 2,
                              static_cast<long long>(k1 + j + 1) << 2);
                double d5 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6;
                double d6 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6 + 4.0;
                initChunkSeed(static_cast<long long>(l1 + i + 1) << 2,
                              static_cast<long long>(k1 + j + 1) << 2);
                double d7 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6 + 4.0;
                double d8 = ((double)NextInt(1024) / 1024.0 - 0.5) * 3.6 + 4.0;
                int k2 = aint[l1 + 1 + (k1 + 0) * k] & 255;
                int l2 = aint[l1 + 1 + (k1 + 1) * k] & 255;

                for (int i3 = 0; i3 < 4; ++i3) {
                    int j3 = ((k1 << 2) + i3) * i1 + (l1 << 2);

                    for (int k3 = 0; k3 < 4; ++k3) {
                        double d9 = ((double)i3 - d2) * ((double)i3 - d2) +
                                    ((double)k3 - d1) * ((double)k3 - d1);
                        double d10 = ((double)i3 - d4) * ((double)i3 - d4) +
                                     ((double)k3 - d3) * ((double)k3 - d3);
                        double d11 = ((double)i3 - d6) * ((double)i3 - d6) +
                                     ((double)k3 - d5) * ((double)k3 - d5);
                        double d12 = ((double)i3 - d8) * ((double)i3 - d8) +
                                     ((double)k3 - d7) * ((double)k3 - d7);

                        if (d9 < d10 && d9 < d11 && d9 < d12) {
                            aint1[j3++] = i2;
                        } else if (d10 < d9 && d10 < d11 && d10 < d12) {
                            aint1[j3++] = k2;
                        } else if (d11 < d9 && d11 < d10 && d11 < d12) {
                            aint1[j3++] = j2;
                        } else {
                            aint1[j3++] = l2;
                        }
                    }
                }

                i2 = k2;
                j2 = l2;
            }
        }

        std::vector<int> aint2 = std::vector<int>(
            areaWidth *
            areaHeight);  // IntCache.getIntCache(areaWidth * areaHeight);

        for (int l3 = 0; l3 < areaHeight; ++l3) {
            std::copy(aint1.begin() + (l3 + (areaY & 3)) * i1 + (areaX & 3),
                      aint1.begin() + (l3 + (areaY & 3)) * i1 + (areaX & 3) +
                          areaWidth,
                      aint2.begin() + l3 * areaWidth);
        }

        return aint2;
    }
};
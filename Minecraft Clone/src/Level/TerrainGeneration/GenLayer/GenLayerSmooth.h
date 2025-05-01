#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerSmooth : public GenLayer {
 public:
  GenLayerSmooth(int64_t baseSeedIn, GenLayer* parentIn)
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
        std::vector<int>(areaWidth * areaHeight);  // int cache thingy

    for (int i1 = 0; i1 < areaHeight; ++i1) {
      for (int j1 = 0; j1 < areaWidth; ++j1) {
        int k1 = aint[j1 + 0 + (i1 + 1) * k];
        int l1 = aint[j1 + 2 + (i1 + 1) * k];
        int i2 = aint[j1 + 1 + (i1 + 0) * k];
        int j2 = aint[j1 + 1 + (i1 + 2) * k];
        int k2 = aint[j1 + 1 + (i1 + 1) * k];

        if (k1 == l1 && i2 == j2) {
          initChunkSeed(static_cast<int64_t>(j1 + areaX),
                        static_cast<int64_t>(i1 + areaY));

          if (NextInt(2) == 0) {
            k2 = k1;
          } else {
            k2 = i2;
          }
        } else {
          if (k1 == l1) {
            k2 = k1;
          }

          if (i2 == j2) {
            k2 = i2;
          }
        }

        aint1[j1 + i1 * areaWidth] = k2;
      }
    }

    return aint1;
  }
};

#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerIsland : public GenLayer {
 private:
 public:
  GenLayerIsland(int64_t baseSeedIn) : GenLayer(baseSeedIn) {}

  std::vector<int> getInts(int areaX, int areaY, int areaWidth,
                           int areaHeight) override {
    std::vector<int> aint = std::vector<int>(areaWidth * areaHeight);

    for (int i = 0; i < areaHeight; ++i) {
      for (int j = 0; j < areaWidth; ++j) {
        initChunkSeed(static_cast<int64_t>(areaX + j),
                      static_cast<int64_t>(areaY + i));
        aint[j + i * areaWidth] = NextInt(10) == 0 ? 1 : 0;
      }
    }

    if (areaX > -areaWidth && areaX <= 0 && areaY > -areaHeight && areaY <= 0) {
      aint[-areaX + -areaY * areaWidth] = 1;
    }

    return aint;
  }
};

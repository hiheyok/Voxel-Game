#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerZoom.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerFuzzyZoom : public GenLayerZoom {
 public:
  GenLayerFuzzyZoom(int64_t baseSeedIn, GenLayer* parentIn)
      : GenLayerZoom(baseSeedIn, parentIn) {}

  int selectModeOrRandom(int p_151617_1_, int p_151617_2_, int p_151617_3_,
                         int p_151617_4_) {
    return selectRandom({p_151617_1_, p_151617_2_, p_151617_3_, p_151617_4_});
  }
};

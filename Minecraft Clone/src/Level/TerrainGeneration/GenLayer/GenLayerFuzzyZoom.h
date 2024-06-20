#pragma once
#include "GenLayer.h"
#include "IntCache.h"
#include "GenLayerZoom.h"

class GenLayerFuzzyZoom : public GenLayerZoom {
public:
	GenLayerFuzzyZoom(long long baseSeedIn, GenLayer* parentIn) : GenLayerZoom(baseSeedIn, parentIn) {
	}

	int selectModeOrRandom(int p_151617_1_, int p_151617_2_, int p_151617_3_, int p_151617_4_)
	{
		return selectRandom({p_151617_1_, p_151617_2_, p_151617_3_, p_151617_4_});
	}
};
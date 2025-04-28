#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerAddIsland.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerAddMushroomIsland.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerAddSnow.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerBiome.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerBiomeEdge.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerDeepOcean.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerEdge.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerFuzzyZoom.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerHills.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerIsland.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerRareBiome.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerRemoveTooMuchOcean.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerRiver.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerRiverInit.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerRiverMix.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerShore.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerSmooth.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerVoronoiZoom.h"
#include "Level/TerrainGeneration/GenLayer/GenLayerZoom.h"

class CombinedGenLayers {
   public:
    static std::vector<GenLayer*> initializeAllBiomeGenerators(
        long long seed,
        ChunkGeneratorSettings* p_180781_3_) {  // TODO: Use unique ptr
        GenLayer* genlayer = new GenLayerIsland(1L);
        genlayer = new GenLayerFuzzyZoom(2000L, genlayer);
        GenLayer* genlayeraddisland = new GenLayerAddIsland(1L, genlayer);
        GenLayer* genlayerzoom = new GenLayerZoom(2001L, genlayeraddisland);
        GenLayer* genlayeraddisland1 = new GenLayerAddIsland(2L, genlayerzoom);
        genlayeraddisland1 = new GenLayerAddIsland(50L, genlayeraddisland1);
        genlayeraddisland1 = new GenLayerAddIsland(70L, genlayeraddisland1);
        GenLayer* genlayerremovetoomuchocean =
            new GenLayerRemoveTooMuchOcean(2LL, genlayeraddisland1);
        GenLayer* genlayeraddsnow =
            new GenLayerAddSnow(2L, genlayerremovetoomuchocean);
        GenLayer* genlayeraddisland2 =
            new GenLayerAddIsland(3L, genlayeraddsnow);
        GenLayer* genlayeredge = new GenLayerEdge(
            2L, genlayeraddisland2, GenLayerEdge::Mode::COOL_WARM);
        genlayeredge =
            new GenLayerEdge(2L, genlayeredge, GenLayerEdge::Mode::HEAT_ICE);
        genlayeredge =
            new GenLayerEdge(3L, genlayeredge, GenLayerEdge::Mode::SPECIAL);
        GenLayer* genlayerzoom1 = new GenLayerZoom(2002L, genlayeredge);
        genlayerzoom1 = new GenLayerZoom(2003L, genlayerzoom1);
        GenLayer* genlayeraddisland3 = new GenLayerAddIsland(4L, genlayerzoom1);
        GenLayer* genlayeraddmushroomisland =
            new GenLayerAddMushroomIsland(5L, genlayeraddisland3);
        GenLayer* genlayerdeepocean =
            new GenLayerDeepOcean(4L, genlayeraddmushroomisland);
        GenLayer* genlayer4 =
            GenLayerZoom::magnify(1000L, genlayerdeepocean, 0);
        int i = 4;
        int j = i;

        if (p_180781_3_ != nullptr) {
            i = p_180781_3_->biomeSize;
            j = p_180781_3_->riverSize;
        }

        // if (p_180781_2_ == WorldType.LARGE_BIOMES)
        //{
        //     i = 6;
        // }

        GenLayer* lvt_7_1_ = GenLayerZoom::magnify(1000LL, genlayer4, 0);
        GenLayer* genlayerriverinit = new GenLayerRiverInit(100LL, lvt_7_1_);
        GenLayer* lvt_8_1_ = new GenLayerBiome(200LL, genlayer4, p_180781_3_);
        GenLayer* genlayer6 = GenLayerZoom::magnify(1000LL, lvt_8_1_, 2);
        GenLayer* genlayerbiomeedge = new GenLayerBiomeEdge(1000LL, genlayer6);
        GenLayer* lvt_9_1_ =
            GenLayerZoom::magnify(1000LL, genlayerriverinit, 2);
        GenLayer* genlayerhills =
            new GenLayerHills(1000LL, genlayerbiomeedge, lvt_9_1_);
        GenLayer* genlayer5 =
            GenLayerZoom::magnify(1000LL, genlayerriverinit, 2);
        genlayer5 = GenLayerZoom::magnify(1000LL, genlayer5, j);
        GenLayer* genlayerriver = new GenLayerRiver(1LL, genlayer5);
        GenLayer* genlayersmooth = new GenLayerSmooth(1000LL, genlayerriver);
        genlayerhills = new GenLayerRareBiome(1001LL, genlayerhills);

        for (int k = 0; k < i; ++k) {
            genlayerhills =
                new GenLayerZoom((long long)(1000 + k), genlayerhills);

            if (k == 0) {
                genlayerhills = new GenLayerAddIsland(3LL, genlayerhills);
            }

            if (k == 1 || i == 1) {
                genlayerhills = new GenLayerShore(1000LL, genlayerhills);
            }
        }

        GenLayer* genlayersmooth1 = new GenLayerSmooth(1000LL, genlayerhills);
        GenLayer* genlayerrivermix =
            new GenLayerRiverMix(100LL, genlayersmooth1, genlayersmooth);
        GenLayer* genlayer3 = new GenLayerVoronoiZoom(10LL, genlayerrivermix);
        genlayerrivermix->initWorldGenSeed(seed);
        genlayer3->initWorldGenSeed(seed);
        return {genlayerrivermix, genlayer3, genlayerrivermix};
    }
};
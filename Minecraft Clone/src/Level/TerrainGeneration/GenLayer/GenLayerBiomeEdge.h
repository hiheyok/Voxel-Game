#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"
#include "Level/TerrainGeneration/Biome/Biomes.h"

class GenLayerBiomeEdge : public GenLayer {
public:
    GenLayerBiomeEdge(long long baseSeedIn, GenLayer* parentIn) : GenLayer(baseSeedIn) {
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

                if (!replaceBiomeEdgeIfNecessary(aint, aint1, j, i, areaWidth, k, Biome::getIdForBiome(Biomes::EXTREME_HILLS), Biome::getIdForBiome(Biomes::EXTREME_HILLS_EDGE)) && !replaceBiomeEdge(aint, aint1, j, i, areaWidth, k, Biome::getIdForBiome(Biomes::MESA_ROCK), Biome::getIdForBiome(Biomes::MESA)) && !replaceBiomeEdge(aint, aint1, j, i, areaWidth, k, Biome::getIdForBiome(Biomes::MESA_CLEAR_ROCK), Biome::getIdForBiome(Biomes::MESA)) && !replaceBiomeEdge(aint, aint1, j, i, areaWidth, k, Biome::getIdForBiome(Biomes::REDWOOD_TAIGA), Biome::getIdForBiome(Biomes::TAIGA)))
                {
                    if (k == Biome::getIdForBiome(Biomes::DESERT))
                    {
                        int l1 = aint[j + 1 + (i + 1 - 1) * (areaWidth + 2)];
                        int i2 = aint[j + 1 + 1 + (i + 1) * (areaWidth + 2)];
                        int j2 = aint[j + 1 - 1 + (i + 1) * (areaWidth + 2)];
                        int k2 = aint[j + 1 + (i + 1 + 1) * (areaWidth + 2)];

                        if (l1 != Biome::getIdForBiome(Biomes::ICE_PLAINS) && i2 != Biome::getIdForBiome(Biomes::ICE_PLAINS) && j2 != Biome::getIdForBiome(Biomes::ICE_PLAINS) && k2 != Biome::getIdForBiome(Biomes::ICE_PLAINS))
                        {
                            aint1[j + i * areaWidth] = k;
                        }
                        else
                        {
                            aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::EXTREME_HILLS_WITH_TREES);
                        }
                    }
                    else if (k == Biome::getIdForBiome(Biomes::SWAMPLAND))
                    {
                        int l = aint[j + 1 + (i + 1 - 1) * (areaWidth + 2)];
                        int i1 = aint[j + 1 + 1 + (i + 1) * (areaWidth + 2)];
                        int j1 = aint[j + 1 - 1 + (i + 1) * (areaWidth + 2)];
                        int k1 = aint[j + 1 + (i + 1 + 1) * (areaWidth + 2)];

                        if (l != Biome::getIdForBiome(Biomes::DESERT) && i1 != Biome::getIdForBiome(Biomes::DESERT) && j1 != Biome::getIdForBiome(Biomes::DESERT) && k1 != Biome::getIdForBiome(Biomes::DESERT) && l != Biome::getIdForBiome(Biomes::COLD_TAIGA) && i1 != Biome::getIdForBiome(Biomes::COLD_TAIGA) && j1 != Biome::getIdForBiome(Biomes::COLD_TAIGA) && k1 != Biome::getIdForBiome(Biomes::COLD_TAIGA) && l != Biome::getIdForBiome(Biomes::ICE_PLAINS) && i1 != Biome::getIdForBiome(Biomes::ICE_PLAINS) && j1 != Biome::getIdForBiome(Biomes::ICE_PLAINS) && k1 != Biome::getIdForBiome(Biomes::ICE_PLAINS))
                        {
                            if (l != Biome::getIdForBiome(Biomes::JUNGLE) && k1 != Biome::getIdForBiome(Biomes::JUNGLE) && i1 != Biome::getIdForBiome(Biomes::JUNGLE) && j1 != Biome::getIdForBiome(Biomes::JUNGLE))
                            {
                                aint1[j + i * areaWidth] = k;
                            }
                            else
                            {
                                aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::JUNGLE_EDGE);
                            }
                        }
                        else
                        {
                            aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::PLAINS);
                        }
                    }
                    else
                    {
                        aint1[j + i * areaWidth] = k;
                    }
                }
            }
        }

        return aint1;
    }

    bool replaceBiomeEdgeIfNecessary(std::vector<int>& p_151636_1_, std::vector<int>& p_151636_2_, int p_151636_3_, int p_151636_4_, int p_151636_5_, int p_151636_6_, int p_151636_7_, int p_151636_8_)
    {
        if (!biomesEqualOrMesaPlateau(p_151636_6_, p_151636_7_))
        {
            return false;
        }
        else
        {
            int i = p_151636_1_[p_151636_3_ + 1 + (p_151636_4_ + 1 - 1) * (p_151636_5_ + 2)];
            int j = p_151636_1_[p_151636_3_ + 1 + 1 + (p_151636_4_ + 1) * (p_151636_5_ + 2)];
            int k = p_151636_1_[p_151636_3_ + 1 - 1 + (p_151636_4_ + 1) * (p_151636_5_ + 2)];
            int l = p_151636_1_[p_151636_3_ + 1 + (p_151636_4_ + 1 + 1) * (p_151636_5_ + 2)];

            if (canBiomesBeNeighbors(i, p_151636_7_) && canBiomesBeNeighbors(j, p_151636_7_) && canBiomesBeNeighbors(k, p_151636_7_) && canBiomesBeNeighbors(l, p_151636_7_))
            {
                p_151636_2_[p_151636_3_ + p_151636_4_ * p_151636_5_] = p_151636_6_;
            }
            else
            {
                p_151636_2_[p_151636_3_ + p_151636_4_ * p_151636_5_] = p_151636_8_;
            }

            return true;
        }
    }

    bool replaceBiomeEdge(std::vector<int>& p_151635_1_, std::vector<int>& p_151635_2_, int p_151635_3_, int p_151635_4_, int p_151635_5_, int p_151635_6_, int p_151635_7_, int p_151635_8_)
    {
        if (p_151635_6_ != p_151635_7_)
        {
            return false;
        }
        else
        {
            int i = p_151635_1_[p_151635_3_ + 1 + (p_151635_4_ + 1 - 1) * (p_151635_5_ + 2)];
            int j = p_151635_1_[p_151635_3_ + 1 + 1 + (p_151635_4_ + 1) * (p_151635_5_ + 2)];
            int k = p_151635_1_[p_151635_3_ + 1 - 1 + (p_151635_4_ + 1) * (p_151635_5_ + 2)];
            int l = p_151635_1_[p_151635_3_ + 1 + (p_151635_4_ + 1 + 1) * (p_151635_5_ + 2)];

            if (biomesEqualOrMesaPlateau(i, p_151635_7_) && biomesEqualOrMesaPlateau(j, p_151635_7_) && biomesEqualOrMesaPlateau(k, p_151635_7_) && biomesEqualOrMesaPlateau(l, p_151635_7_))
            {
                p_151635_2_[p_151635_3_ + p_151635_4_ * p_151635_5_] = p_151635_6_;
            }
            else
            {
                p_151635_2_[p_151635_3_ + p_151635_4_ * p_151635_5_] = p_151635_8_;
            }

            return true;
        }
    }

    bool canBiomesBeNeighbors(int p_151634_1_, int p_151634_2_)
    {
        if (biomesEqualOrMesaPlateau(p_151634_1_, p_151634_2_))
        {
            return true;
        }
        else
        {
            Biome* biome = Biome::getBiome(p_151634_1_);
            Biome* biome1 = Biome::getBiome(p_151634_2_);

            if (biome != nullptr && biome1 != nullptr)
            {
                Biome::TempCategory biomeTempcategory = biome->getTempCategory();
                Biome::TempCategory biomeTempcategory1 = biome1->getTempCategory();
                return biomeTempcategory == biomeTempcategory1 || biomeTempcategory == Biome::TempCategory::MEDIUM || biomeTempcategory1 == Biome::TempCategory::MEDIUM;
            }
            else
            {
                return false;
            }
        }
    }
};
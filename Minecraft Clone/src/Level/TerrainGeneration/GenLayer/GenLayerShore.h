#pragma once
#include "Level/TerrainGeneration/GenLayer/GenLayer.h"
#include "Level/TerrainGeneration/GenLayer/IntCache.h"

class GenLayerShore : public GenLayer {
   private:
    void replaceIfNeighborOcean(std::vector<int>& p_151632_1_,
                                std::vector<int>& p_151632_2_, int p_151632_3_,
                                int p_151632_4_, int p_151632_5_,
                                int p_151632_6_, int p_151632_7_) {
        if (isBiomeOceanic(p_151632_6_)) {
            p_151632_2_[p_151632_3_ + p_151632_4_ * p_151632_5_] = p_151632_6_;
        } else {
            int i = p_151632_1_[p_151632_3_ + 1 +
                                (p_151632_4_ + 1 - 1) * (p_151632_5_ + 2)];
            int j = p_151632_1_[p_151632_3_ + 1 + 1 +
                                (p_151632_4_ + 1) * (p_151632_5_ + 2)];
            int k = p_151632_1_[p_151632_3_ + 1 - 1 +
                                (p_151632_4_ + 1) * (p_151632_5_ + 2)];
            int l = p_151632_1_[p_151632_3_ + 1 +
                                (p_151632_4_ + 1 + 1) * (p_151632_5_ + 2)];

            if (!isBiomeOceanic(i) && !isBiomeOceanic(j) &&
                !isBiomeOceanic(k) && !isBiomeOceanic(l)) {
                p_151632_2_[p_151632_3_ + p_151632_4_ * p_151632_5_] =
                    p_151632_6_;
            } else {
                p_151632_2_[p_151632_3_ + p_151632_4_ * p_151632_5_] =
                    p_151632_7_;
            }
        }
    }

    bool isJungleCompatible(int p_151631_1_) {
        if (Biome::getBiome(p_151631_1_) != nullptr &&
            Biome::getBiome(p_151631_1_)->getBiomeClass() ==
                typeid(BiomeJungle))  // class thingy
        {
            return true;
        } else {
            return p_151631_1_ == Biome::getIdForBiome(Biomes::JUNGLE_EDGE) ||
                   p_151631_1_ == Biome::getIdForBiome(Biomes::JUNGLE) ||
                   p_151631_1_ == Biome::getIdForBiome(Biomes::JUNGLE_HILLS) ||
                   p_151631_1_ == Biome::getIdForBiome(Biomes::FOREST) ||
                   p_151631_1_ == Biome::getIdForBiome(Biomes::TAIGA) ||
                   isBiomeOceanic(p_151631_1_);
        }
    }

    bool isMesa(int p_151633_1_) {
        return dynamic_cast<BiomeMesa*>(Biome::getBiome(p_151633_1_)) !=
               nullptr;
    }

   public:
    GenLayerShore(long long baseSeedIn, GenLayer* parentIn)
        : GenLayer(baseSeedIn) {
        parent = parentIn;
    }

    std::vector<int> getInts(int areaX, int areaY, int areaWidth,
                             int areaHeight) override {
        std::vector<int> aint = parent->getInts(areaX - 1, areaY - 1,
                                                areaWidth + 2, areaHeight + 2);
        std::vector<int> aint1 = std::vector<int>(areaWidth * areaHeight);

        for (int i = 0; i < areaHeight; ++i) {
            for (int j = 0; j < areaWidth; ++j) {
                initChunkSeed((long long)(j + areaX), (long long)(i + areaY));
                int k = aint[j + 1 + (i + 1) * (areaWidth + 2)];
                Biome* biome = Biome::getBiome(k);

                if (k == Biome::getIdForBiome(Biomes::MUSHROOM_ISLAND)) {
                    int j2 = aint[j + 1 + (i + 1 - 1) * (areaWidth + 2)];
                    int i3 = aint[j + 1 + 1 + (i + 1) * (areaWidth + 2)];
                    int l3 = aint[j + 1 - 1 + (i + 1) * (areaWidth + 2)];
                    int k4 = aint[j + 1 + (i + 1 + 1) * (areaWidth + 2)];

                    if (j2 != Biome::getIdForBiome(Biomes::OCEAN) &&
                        i3 != Biome::getIdForBiome(Biomes::OCEAN) &&
                        l3 != Biome::getIdForBiome(Biomes::OCEAN) &&
                        k4 != Biome::getIdForBiome(Biomes::OCEAN)) {
                        aint1[j + i * areaWidth] = k;
                    } else {
                        aint1[j + i * areaWidth] =
                            Biome::getIdForBiome(Biomes::MUSHROOM_ISLAND_SHORE);
                    }
                }

                else if (biome != nullptr &&
                         biome->getBiomeClass() == typeid(BiomeJungle)) {
                    int i2 = aint[j + 1 + (i + 1 - 1) * (areaWidth + 2)];
                    int l2 = aint[j + 1 + 1 + (i + 1) * (areaWidth + 2)];
                    int k3 = aint[j + 1 - 1 + (i + 1) * (areaWidth + 2)];
                    int j4 = aint[j + 1 + (i + 1 + 1) * (areaWidth + 2)];

                    if (isJungleCompatible(i2) && isJungleCompatible(l2) &&
                        isJungleCompatible(k3) && isJungleCompatible(j4)) {
                        if (!isBiomeOceanic(i2) && !isBiomeOceanic(l2) &&
                            !isBiomeOceanic(k3) && !isBiomeOceanic(j4)) {
                            aint1[j + i * areaWidth] = k;
                        } else {
                            aint1[j + i * areaWidth] =
                                Biome::getIdForBiome(Biomes::BEACH);
                        }
                    } else {
                        aint1[j + i * areaWidth] =
                            Biome::getIdForBiome(Biomes::JUNGLE_EDGE);
                    }
                } else if (k != Biome::getIdForBiome(Biomes::EXTREME_HILLS) &&
                           k != Biome::getIdForBiome(
                                    Biomes::EXTREME_HILLS_WITH_TREES) &&
                           k != Biome::getIdForBiome(
                                    Biomes::EXTREME_HILLS_EDGE)) {
                    if (biome != nullptr && biome->isSnowyBiome()) {
                        replaceIfNeighborOcean(
                            aint, aint1, j, i, areaWidth, k,
                            Biome::getIdForBiome(Biomes::COLD_BEACH));
                    } else if (k != Biome::getIdForBiome(Biomes::MESA) &&
                               k != Biome::getIdForBiome(Biomes::MESA_ROCK)) {
                        if (k != Biome::getIdForBiome(Biomes::OCEAN) &&
                            k != Biome::getIdForBiome(Biomes::DEEP_OCEAN) &&
                            k != Biome::getIdForBiome(Biomes::RIVER) &&
                            k != Biome::getIdForBiome(Biomes::SWAMPLAND)) {
                            int l1 =
                                aint[j + 1 + (i + 1 - 1) * (areaWidth + 2)];
                            int k2 =
                                aint[j + 1 + 1 + (i + 1) * (areaWidth + 2)];
                            int j3 =
                                aint[j + 1 - 1 + (i + 1) * (areaWidth + 2)];
                            int i4 =
                                aint[j + 1 + (i + 1 + 1) * (areaWidth + 2)];

                            if (!isBiomeOceanic(l1) && !isBiomeOceanic(k2) &&
                                !isBiomeOceanic(j3) && !isBiomeOceanic(i4)) {
                                aint1[j + i * areaWidth] = k;
                            } else {
                                aint1[j + i * areaWidth] =
                                    Biome::getIdForBiome(Biomes::BEACH);
                            }
                        } else {
                            aint1[j + i * areaWidth] = k;
                        }
                    } else {
                        int l = aint[j + 1 + (i + 1 - 1) * (areaWidth + 2)];
                        int i1 = aint[j + 1 + 1 + (i + 1) * (areaWidth + 2)];
                        int j1 = aint[j + 1 - 1 + (i + 1) * (areaWidth + 2)];
                        int k1 = aint[j + 1 + (i + 1 + 1) * (areaWidth + 2)];

                        if (!isBiomeOceanic(l) && !isBiomeOceanic(i1) &&
                            !isBiomeOceanic(j1) && !isBiomeOceanic(k1)) {
                            if (isMesa(l) && isMesa(i1) && isMesa(j1) &&
                                isMesa(k1)) {
                                aint1[j + i * areaWidth] = k;
                            } else {
                                aint1[j + i * areaWidth] =
                                    Biome::getIdForBiome(Biomes::DESERT);
                            }
                        } else {
                            aint1[j + i * areaWidth] = k;
                        }
                    }
                } else {
                    replaceIfNeighborOcean(
                        aint, aint1, j, i, areaWidth, k,
                        Biome::getIdForBiome(Biomes::STONE_BEACH));
                }
            }
        }

        return aint1;
    }
};
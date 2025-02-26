#pragma once
#include <vector>
#include "../Biome/Biomes.h"

class GenLayer {
public:

	long long int baseSeed = 0, chunkSeed = 0, worldGenSeed = 0;

	GenLayer* parent = nullptr;

	GenLayer(long long int baseSeedIn) {
        baseSeed = baseSeedIn;
        baseSeed *= baseSeed * 6364136223846793005LL + 1442695040888963407L;
        baseSeed += baseSeedIn;
        baseSeed *= baseSeed * 6364136223846793005LL + 1442695040888963407L;
        baseSeed += baseSeedIn;
        baseSeed *= baseSeed * 6364136223846793005LL + 1442695040888963407L;
        baseSeed += baseSeedIn;
	}

	GenLayer() {

	}
	

	virtual std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) {
        throw std::exception("no");
	}

    void initChunkSeed(long long val1, long long val2) {
        chunkSeed = worldGenSeed;
        chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
        chunkSeed += val1;
        chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
        chunkSeed += val2;
        chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
        chunkSeed += val1;
        chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
        chunkSeed += val2;
    }

    virtual void initWorldGenSeed(long long seed) {
        worldGenSeed = seed;

        if (parent != nullptr) {
            parent->initWorldGenSeed(seed);
        }

        worldGenSeed *= worldGenSeed * 6364136223846793005LL + 1442695040888963407LL;
        worldGenSeed += baseSeed;
        worldGenSeed *= worldGenSeed * 6364136223846793005LL + 1442695040888963407LL;
        worldGenSeed += baseSeed;
        worldGenSeed *= worldGenSeed * 6364136223846793005LL + 1442695040888963407LL;
        worldGenSeed += baseSeed;
    }

    int nextInt(int rngSeed) {
        int i = (int)((chunkSeed >> 24) % (long long)rngSeed);

        if (i < 0) {
            i += rngSeed;
        }

        chunkSeed *= chunkSeed * 6364136223846793005LL + 1442695040888963407LL;
        chunkSeed += worldGenSeed;
        return i;
    }

    static bool biomesEqualOrMesaPlateau(int biomeIDA, int biomeIDB)
    {
        if (biomeIDA == biomeIDB) {
            return true;
        } else {
            Biome* biome = Biome::getBiome(biomeIDA);
            Biome* biome1 = Biome::getBiome(biomeIDB);

            if (biome != nullptr && biome1 != nullptr)
            {
                if (biome != Biomes::MESA_ROCK && biome != Biomes::MESA_CLEAR_ROCK)
                {
                    return biome == biome1 || biome->getBiomeClass() == biome1->getBiomeClass(); //return biome == biome1 || biome->getBiomeClass() == biome1->getBiomeClass();
                } //????
                else
                {
                    return biome1 == Biomes::MESA_ROCK || biome1 == Biomes::MESA_CLEAR_ROCK;
                }
            }
            else
            {
                return false;
            }
        }
    }

    int selectRandom(std::vector<int> rngNum) {
        return rngNum[nextInt(static_cast<int>(rngNum.size()))];
    }

    int selectModeOrRandom(int p_151617_1_, int p_151617_2_, int p_151617_3_, int p_151617_4_) {
        if (p_151617_2_ == p_151617_3_ && p_151617_3_ == p_151617_4_)
        {
            return p_151617_2_;
        }
        else if (p_151617_1_ == p_151617_2_ && p_151617_1_ == p_151617_3_)
        {
            return p_151617_1_;
        }
        else if (p_151617_1_ == p_151617_2_ && p_151617_1_ == p_151617_4_)
        {
            return p_151617_1_;
        }
        else if (p_151617_1_ == p_151617_3_ && p_151617_1_ == p_151617_4_)
        {
            return p_151617_1_;
        }
        else if (p_151617_1_ == p_151617_2_ && p_151617_3_ != p_151617_4_)
        {
            return p_151617_1_;
        }
        else if (p_151617_1_ == p_151617_3_ && p_151617_2_ != p_151617_4_)
        {
            return p_151617_1_;
        }
        else if (p_151617_1_ == p_151617_4_ && p_151617_2_ != p_151617_3_)
        {
            return p_151617_1_;
        }
        else if (p_151617_2_ == p_151617_3_ && p_151617_1_ != p_151617_4_)
        {
            return p_151617_2_;
        }
        else if (p_151617_2_ == p_151617_4_ && p_151617_1_ != p_151617_3_)
        {
            return p_151617_2_;
        }
        else
        {
            return p_151617_3_ == p_151617_4_ && p_151617_1_ != p_151617_2_ ? p_151617_3_ : selectRandom({ p_151617_1_, p_151617_2_, p_151617_3_, p_151617_4_ });
        }
    }

    static bool isBiomeOceanic(int p_151618_0_) {
        Biome* biome = Biome::getBiome(p_151618_0_);
        return biome == Biomes::OCEAN || biome == Biomes::DEEP_OCEAN || biome == Biomes::FROZEN_OCEAN;
    }
};
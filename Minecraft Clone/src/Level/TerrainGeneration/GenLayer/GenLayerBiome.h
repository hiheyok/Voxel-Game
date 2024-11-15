#pragma once
#include "GenLayer.h"
#include "IntCache.h"
#include "../Noisemaps/TerrainParameters.h"
#include "../Biome/Biomes.h"

class GenLayerBiome : public GenLayer {
private:
    std::vector<Biome*> warmBiomes = { Biomes::DESERT, Biomes::DESERT, Biomes::DESERT, Biomes::SAVANNA, Biomes::SAVANNA, Biomes::PLAINS };
    std::vector<Biome*> mediumBiomes = { Biomes::FOREST, Biomes::ROOFED_FOREST, Biomes::EXTREME_HILLS, Biomes::PLAINS, Biomes::BIRCH_FOREST, Biomes::SWAMPLAND };
    std::vector<Biome*> iceBiomes = { Biomes::ICE_PLAINS, Biomes::ICE_PLAINS, Biomes::ICE_PLAINS, Biomes::COLD_TAIGA };
    std::vector<Biome*> coldBiomes = { Biomes::FOREST, Biomes::EXTREME_HILLS, Biomes::TAIGA, Biomes::PLAINS };
    ChunkGeneratorSettings* settings;
public:

	GenLayerBiome(long long baseSeedIn, GenLayer* parentIn, ChunkGeneratorSettings* settingsIn) : GenLayer(baseSeedIn) { // add worldtype ;ater
        parent = parentIn;
        settings = settingsIn;

	}

    std::vector<int> getInts(int areaX, int areaY, int areaWidth, int areaHeight) override
    {
        std::vector<int>  aint = parent->getInts(areaX, areaY, areaWidth, areaHeight);
        std::vector<int>  aint1 = std::vector<int>(areaWidth * areaHeight); //int cache

        for (int i = 0; i < areaHeight; ++i)
        {
            for (int j = 0; j < areaWidth; ++j)
            {
                initChunkSeed((long long)(j + areaX), (long long)(i + areaY));
                int k = aint[j + i * areaWidth];
                int l = (k & 3840) >> 8;
                k = k & -3841;

                if (settings != nullptr && settings->fixedBiome >= 0)
                {
                    aint1[j + i * areaWidth] = settings->fixedBiome;
                }
                else if (isBiomeOceanic(k))
                {
                    aint1[j + i * areaWidth] = k;
                }
                else if (k == Biome::getIdForBiome(Biomes::MUSHROOM_ISLAND))
                {
                    aint1[j + i * areaWidth] = k;
                }
                else if (k == 1)
                {
                    if (l > 0)
                    {
                        if (nextInt(3) == 0)
                        {
                            aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::MESA_CLEAR_ROCK);
                        }
                        else
                        {
                            aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::MESA_ROCK);
                        }
                    }
                    else
                    {
                        aint1[j + i * areaWidth] = Biome::getIdForBiome(warmBiomes[nextInt(warmBiomes.size())]);
                    }
                }
                else if (k == 2)
                {
                    if (l > 0)
                    {
                        aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::JUNGLE);
                    }
                    else
                    {
                        aint1[j + i * areaWidth] = Biome::getIdForBiome(mediumBiomes[nextInt(mediumBiomes.size())]);
                    }
                }
                else if (k == 3)
                {
                    if (l > 0)
                    {
                        aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::REDWOOD_TAIGA);
                    }
                    else
                    {
                        aint1[j + i * areaWidth] = Biome::getIdForBiome(coldBiomes[nextInt(coldBiomes.size())]);
                    }
                }
                else if (k == 4)
                {
                    aint1[j + i * areaWidth] = Biome::getIdForBiome(iceBiomes[nextInt(iceBiomes.size())]);
                }
                else
                {
                    aint1[j + i * areaWidth] = Biome::getIdForBiome(Biomes::MUSHROOM_ISLAND);
                }
            }
        }

        return aint1;
    }
};
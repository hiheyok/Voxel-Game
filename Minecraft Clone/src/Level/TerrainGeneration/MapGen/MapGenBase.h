#pragma once
#include "../Random/JavaRandom.h"
#include "../../Chunk/TallChunk.h"
#include "../../TerrainGeneration/Generator.h"

class MapGenBase {
protected:
	JavaRandom rand = JavaRandom();
    int range = 0;

    void generate(int x, int z, TallChunk* chunk)
    {
        int i = range;
        rand.setSeed(WorldGenerator::worldSeed);
        long long j = rand.nextLong();
        long long k = rand.nextLong();

        for (int l = x - i; l <= x + i; ++l)
        {
            for (int i1 = z - i; i1 <= z + i; ++i1)
            {
                long long j1 = (long long)l * j;
                long long k1 = (long long)i1 * k;
                rand.setSeed(j1 ^ k1 ^ WorldGenerator::worldSeed);
                recursiveGenerate(l, i1, x, z, chunk);
            }
        }
    }

    static void setupChunkSeed(long long seed, JavaRandom randIn, int p_191068_3_, int p_191068_4_)
    {
        randIn.setSeed(seed);
        long i = randIn.nextLong();
        long j = randIn.nextLong();
        long k = (long)p_191068_3_ * i;
        long l = (long)p_191068_4_ * j;
        randIn.setSeed(k ^ l ^ seed);
    }

    virtual void recursiveGenerate(int chunkX, int chunkZ, int originalX, int originalZ, TallChunk* chunk)
    {
    }
};
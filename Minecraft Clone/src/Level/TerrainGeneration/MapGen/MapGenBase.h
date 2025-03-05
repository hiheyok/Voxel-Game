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
        rand.SetSeed(WorldGenerator::world_seed_);
        long long j = rand.NextLong();
        long long k = rand.NextLong();

        for (int l = x - i; l <= x + i; ++l)
        {
            for (int i1 = z - i; i1 <= z + i; ++i1)
            {
                long long j1 = (long long)l * j;
                long long k1 = (long long)i1 * k;
                rand.SetSeed(j1 ^ k1 ^ WorldGenerator::world_seed_);
                recursiveGenerate(l, i1, x, z, chunk);
            }
        }
    }

    static void setupChunkSeed(long long seed, JavaRandom randIn, int p_191068_3_, int p_191068_4_)
    {
        randIn.SetSeed(seed);
        long i = randIn.NextLong();
        long j = randIn.NextLong();
        long k = (long)p_191068_3_ * i;
        long l = (long)p_191068_4_ * j;
        randIn.SetSeed(k ^ l ^ seed);
    }

    virtual void recursiveGenerate(int chunkX, int chunkZ, int originalX, int originalZ, TallChunk* chunk)
    {
    }
};
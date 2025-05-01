#pragma once
#include "Level/Chunk/TallChunk.h"
#include "Level/TerrainGeneration/ChunkGenerator.h"
#include "Level/TerrainGeneration/Random/JavaRandom.h"

class MapGenBase {
 protected:
  JavaRandom rand = JavaRandom();
  int range = 0;

  void generate(int x, int z, TallChunk* chunk) {
    int i = range;
    rand.SetSeed(WorldGenerator::world_seed_);
    int64_t j = rand.NextLong();
    int64_t k = rand.NextLong();

    for (int l = x - i; l <= x + i; ++l) {
      for (int i1 = z - i; i1 <= z + i; ++i1) {
        int64_t j1 = static_cast<int64_t>(l) * j;
        int64_t k1 = static_cast<int64_t>(i1) * k;
        rand.SetSeed(j1 ^ k1 ^ WorldGenerator::world_seed_);
        recursiveGenerate(l, i1, x, z, chunk);
      }
    }
  }

  static void setupChunkSeed(int64_t seed, JavaRandom randIn, int p_191068_3_,
                             int p_191068_4_) {
    randIn.SetSeed(seed);
    long i = randIn.NextLong();
    long j = randIn.NextLong();
    long k = (long)p_191068_3_ * i;
    long l = (long)p_191068_4_ * j;
    randIn.SetSeed(k ^ l ^ seed);
  }

  virtual void recursiveGenerate(int chunkX, int chunkZ, int originalX,
                                 int originalZ, TallChunk* chunk) {}
};

#include "Overworld.h"

#include <vector>

#include "Level/TerrainGeneration/Biome/BiomeProvider.h"

void OverworldGenerator::GenerateChunk(int x, int z, TallChunk* chunk) {
  rand.SetSeed(static_cast<int64_t>(x) * 341873128712LL +
               static_cast<int64_t>(z) * 132897987541LL);
  setBlocksInChunk(x, z, chunk);
  biomesForGeneration = BiomeProvider::getBiomes(x * 16, z * 16, 16, 16);
  replaceBiomeBlocks(x, z, chunk, biomesForGeneration);
}

void OverworldGenerator::replaceBiomeBlocks(int x, int z, TallChunk* chunk,
                                            std::vector<Biome*> biomesIn) {
  depthBuffer = surfaceNoise.getRegion(depthBuffer, static_cast<double>(x * 16),
                                       static_cast<double>(z * 16), 16, 16,
                                       0.0625, 0.0625, 1.0);

  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 16; ++j) {
      Biome* biome = biomesIn[j + i * 16];
      biome->GenTerrainBlocks(rand, chunk, x * 16 + i, z * 16 + j,
                              depthBuffer[j + i * 16], &settings_);
    }
  }
}

void OverworldGenerator::generateHeightmap(int x, int y, int z) {
  depthRegion = depthNoise.generateNoiseOctaves(
      depthRegion, x, z, 5, 5, settings_.depthNoiseScaleX,
      settings_.depthNoiseScaleZ, settings_.depthNoiseScaleExponent);
  float f = settings_.coordinateScale;
  float f1 = settings_.stretchY;
  mainNoiseRegion = mainPerlinNoise.generateNoiseOctaves(
      mainNoiseRegion, x, y, z, 5, 33, 5,
      static_cast<double>(f / settings_.mainNoiseScaleX),
      static_cast<double>(f1 / settings_.mainNoiseScaleY),
      static_cast<double>(f / settings_.mainNoiseScaleZ));
  minLimitRegion = minLimitPerlinNoise.generateNoiseOctaves(
      minLimitRegion, x, y, z, 5, 33, 5, static_cast<double>(f),
      static_cast<double>(f1), static_cast<double>(f));
  maxLimitRegion = maxLimitPerlinNoise.generateNoiseOctaves(
      maxLimitRegion, x, y, z, 5, 33, 5, static_cast<double>(f),
      static_cast<double>(f1), static_cast<double>(f));
  int i = 0;
  int j = 0;

  for (int k = 0; k < 5; ++k) {
    for (int l = 0; l < 5; ++l) {
      float f2 = 0.0F;
      float f3 = 0.0F;
      float f4 = 0.0F;
      // int i1 = 2;
      Biome* biome = biomesForGeneration[k + 2 + (l + 2) * 10];

      for (int j1 = -2; j1 <= 2; ++j1) {
        for (int k1 = -2; k1 <= 2; ++k1) {
          Biome* biome1 = biomesForGeneration[k + j1 + 2 + (l + k1 + 2) * 10];
          float f5 = settings_.biomeDepthOffset +
                     biome1->getBaseHeight() * settings_.biomeDepthWeight;
          float f6 = settings_.biomeScaleOffset +
                     biome1->getHeightVariation() * settings_.biomeScaleWeight;

          float f7 = biomeWeights[j1 + 2 + (k1 + 2) * 5] / (f5 + 2.0F);

          if (biome1->getBaseHeight() > biome->getBaseHeight()) {
            f7 /= 2.0F;
          }

          f2 += f6 * f7;
          f3 += f5 * f7;
          f4 += f7;
        }
      }

      f2 = f2 / f4;
      f3 = f3 / f4;
      f2 = f2 * 0.9F + 0.1F;
      f3 = (f3 * 4.0F - 1.0F) / 8.0F;
      double d7 = depthRegion[j] / 8000.0;
      if (d7 < 0.0) {
        d7 = -d7 * 0.3;
      }

      d7 = d7 * 3.0 - 2.0;

      if (d7 < 0.0) {
        d7 = d7 / 2.0;

        if (d7 < -1.0) {
          d7 = -1.0;
        }

        d7 = d7 / 1.4;
        d7 = d7 / 2.0;
      } else {
        if (d7 > 1.0) {
          d7 = 1.0;
        }

        d7 = d7 / 8.0;
      }

      ++j;
      double d8 = static_cast<double>(f3);
      double d9 = static_cast<double>(f2);
      d8 = d8 + d7 * 0.2;
      d8 = d8 * static_cast<double>(settings_.baseSize) / 8.0;
      double d0 = static_cast<double>(settings_.baseSize) + d8 * 4.0;

      for (int l1 = 0; l1 < 33; ++l1) {
        double d1 = (static_cast<double>(l1) - d0) *
                    static_cast<double>(settings_.stretchY) * 128.0 / 256.0 /
                    d9;

        if (d1 < 0.0) {
          d1 *= 4.0;
        }

        double d2 = minLimitRegion[i] / settings_.lowerLimitScale;
        double d3 = maxLimitRegion[i] / settings_.upperLimitScale;
        double d4 = (mainNoiseRegion[i] / 10.0 + 1.0) / 2.0;
        double d5 = clampedLerp(d2, d3, d4) - d1;

        if (l1 > 29) {
          double d6 = static_cast<double>(static_cast<float>(l1 - 29) / 3.0F);
          d5 = d5 * (1.0 - d6) + -10.0 * d6;
        }

        heightMap[i] = d5;
        ++i;
      }
    }
  }
}

void OverworldGenerator::setBlocksInChunk(int x, int z, TallChunk* chunk) {
  biomesForGeneration =
      BiomeProvider::getBiomesForGeneration(x * 4 - 2, z * 4 - 2, 10, 10);
  generateHeightmap(x * 4, 0, z * 4);

  for (int i = 0; i < 4; ++i) {
    int j = i * 5;
    int k = (i + 1) * 5;

    for (int l = 0; l < 4; ++l) {
      int i1 = (j + l) * 33;
      int j1 = (j + l + 1) * 33;
      int k1 = (k + l) * 33;
      int l1 = (k + l + 1) * 33;

      for (int i2 = 0; i2 < 32; ++i2) {
        // double d0 = 0.125;
        double d1 = heightMap[i1 + i2];
        double d2 = heightMap[j1 + i2];
        double d3 = heightMap[k1 + i2];
        double d4 = heightMap[l1 + i2];
        double d5 = (heightMap[i1 + i2 + 1] - d1) * 0.125;
        double d6 = (heightMap[j1 + i2 + 1] - d2) * 0.125;
        double d7 = (heightMap[k1 + i2 + 1] - d3) * 0.125;
        double d8 = (heightMap[l1 + i2 + 1] - d4) * 0.125;

        for (int j2 = 0; j2 < 8; ++j2) {
          // double d9 = 0.25;
          double d10 = d1;
          double d11 = d2;
          double d12 = (d3 - d1) * 0.25;
          double d13 = (d4 - d2) * 0.25;

          for (int k2 = 0; k2 < 4; ++k2) {
            // double d14 = 0.25;
            double d16 = (d11 - d10) * 0.25;
            double lvt_45_1_ = d10 - d16;

            for (int l2 = 0; l2 < 4; ++l2) {
              if ((lvt_45_1_ += d16) > 0.0) {
                int genHeight = i2 * 8 + j2;
                chunk->SetBlockUnsafe(i * 4 + k2, genHeight, l * 4 + l2,
                                      g_blocks.STONE);
              } else if (i2 * 8 + j2 < settings_.seaLevel) {
                int genHeight = i2 * 8 + j2;
                chunk->SetBlockUnsafe(i * 4 + k2, genHeight, l * 4 + l2,
                                      g_blocks.WATER);
              }
            }

            d10 += d12;
            d11 += d13;
          }

          d1 += d5;
          d2 += d6;
          d3 += d7;
          d4 += d8;
        }
      }
    }
  }
}

double OverworldGenerator::clampedLerp(double lowerBnd, double upperBnd,
                                       double slide) {
  if (slide < 0.0) {
    return lowerBnd;
  } else {
    return slide > 1.0 ? upperBnd : lowerBnd + (upperBnd - lowerBnd) * slide;
  }
}

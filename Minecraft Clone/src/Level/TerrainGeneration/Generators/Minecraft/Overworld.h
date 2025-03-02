#pragma once
#include "../../Random/JavaRandom.h"
#include "../../Noisemaps/NoiseOctave.h"
#include "../../Noisemaps/MinecraftNoiseGeneratorPerlin.h"
#include "../../Noisemaps/TerrainParameters.h"
#include "../../../Chunk/Chunk.h"
#include "../../../Chunk/TallChunk.h"
#include "../../../Chunk/Heightmap/Heightmap.h"
#include "../../../Chunk/ChunkColumn.h"
#include "../../Biome/Biome.h"

class OverworldGenerator {
private:

    static double clampedLerp(double lowerBnd, double upperBnd, double slide);

public:
	JavaRandom rand;
    NoiseOctave minLimitPerlinNoise;
    NoiseOctave maxLimitPerlinNoise;
    NoiseOctave mainPerlinNoise;
    NoiseGeneratorPerlin surfaceNoise;
    NoiseOctave scaleNoise;
    NoiseOctave depthNoise;
    std::vector<double> depthBuffer;
    std::vector<double> heightMap;
    std::vector<double> mainNoiseRegion;
    std::vector<double> minLimitRegion;
    std::vector<double> maxLimitRegion;
    std::vector<double> depthRegion;
    ChunkGeneratorSettings settings_;

    std::vector<float> biomeWeights;
    std::vector<Biome*> biomesForGeneration;

    OverworldGenerator(int64_t seed, ChunkGeneratorSettings setting) {
        settings_ = setting;

        rand = JavaRandom(seed);
        minLimitPerlinNoise = NoiseOctave(rand, 16);
        maxLimitPerlinNoise = NoiseOctave(rand, 16);
        mainPerlinNoise = NoiseOctave(rand, 8);
        surfaceNoise = NoiseGeneratorPerlin(rand, 4);
        scaleNoise = NoiseOctave(rand, 10);
        depthNoise = NoiseOctave(rand, 16);
        biomeWeights = std::vector<float>(25);
        heightMap = std::vector<double>(825);
        depthBuffer = std::vector<double>(256);

        for (int i = -2; i <= 2; ++i) {
            for (int j = -2; j <= 2; ++j) {
                float f = 10.0f / sqrtf((float)(i * i + j * j) + 0.2f);
                biomeWeights[i + 2 + (j + 2) * 5] = f;
            }
        }
    }

    void replaceBiomeBlocks(int x, int z, TallChunk* chunk, std::vector<Biome*> biomesIn);

    void GenerateChunk(int x, int z, TallChunk* chunk);

    void generateHeightmap(int x, int y, int z);

    void setBlocksInChunk(int x, int z, TallChunk* chunk);
};
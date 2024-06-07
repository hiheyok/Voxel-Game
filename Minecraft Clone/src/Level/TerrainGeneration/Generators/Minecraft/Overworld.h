#pragma once
#include "../../Random/JavaRandom.h"
#include "../../Noisemaps/NoiseOctave.h"
#include "../../Noisemaps/MinecraftNoiseGeneratorPerlin.h"
#include "../../Noisemaps/TerrainParameters.h"
#include "../../../Chunk/Chunk.h"

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
    std::vector<double> heightMap;
    std::vector<double> mainNoiseRegion;
    std::vector<double> minLimitRegion;
    std::vector<double> maxLimitRegion;
    std::vector<double> depthRegion;
    ChunkGeneratorSettings settings;

    OverworldGenerator(int64_t seed, ChunkGeneratorSettings setting) {
        settings = setting;

        rand = JavaRandom(seed);
        minLimitPerlinNoise = NoiseOctave(rand, 16);
        maxLimitPerlinNoise = NoiseOctave(rand, 16);
        mainPerlinNoise = NoiseOctave(rand, 8);
        surfaceNoise = NoiseGeneratorPerlin(rand, 4);
        scaleNoise = NoiseOctave(rand, 10);
        depthNoise = NoiseOctave(rand, 16);
        
        heightMap = std::vector<double>(825);
    }

    void generateHeightmap(int x, int y, int z);

    void setBlocksInChunk(int x, int z, Chunk* chunk);

};
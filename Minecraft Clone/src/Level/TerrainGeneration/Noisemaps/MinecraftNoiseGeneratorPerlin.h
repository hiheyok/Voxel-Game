#pragma once
#include "MinecraftNoiseGeneratorSimplex.h"

//copy paste mc src ported to c++


class NoiseGeneratorPerlin
{

private:
    std::vector<NoiseGeneratorSimplex> noiseLevels;
    int levels;

public:

    ~NoiseGeneratorPerlin() {
    }

    NoiseGeneratorPerlin(JavaRandom seed, int levelsIn) {
        levels = levelsIn;
        noiseLevels = std::vector<NoiseGeneratorSimplex>(levelsIn);

        for (int i = 0; i < levelsIn; ++i)
        {
            noiseLevels[i] = NoiseGeneratorSimplex(seed);
        }
    }

    NoiseGeneratorPerlin() {

    }

    double getValue(double p_151601_1_, double p_151601_3_);

    std::vector<double> getRegion(std::vector<double>& p_151599_1_, double p_151599_2_, double p_151599_4_, int p_151599_6_, int p_151599_7_, double p_151599_8_, double p_151599_10_, double p_151599_12_);

    std::vector<double> getRegion(std::vector<double>& p_151600_1_, double p_151600_2_, double p_151600_4_, int p_151600_6_, int p_151600_7_, double p_151600_8_, double p_151600_10_, double p_151600_12_, double p_151600_14_);
};

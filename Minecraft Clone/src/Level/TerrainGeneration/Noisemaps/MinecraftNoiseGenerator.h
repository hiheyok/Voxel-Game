#pragma once
#include "../Random/JavaRandom.h"
//directly copied from mc source code


class NoiseGeneratorImproved {
private:
    std::vector<int> permutations;

public:
    double xCoord;
    double yCoord;
    double zCoord;
    
    ~NoiseGeneratorImproved() {
    }

    NoiseGeneratorImproved(JavaRandom& random) {

        permutations.resize(512);
        xCoord = random.NextDouble() * 256.0;
        yCoord = random.NextDouble() * 256.0;
        zCoord = random.NextDouble() * 256.0;

        for (int i = 0; i < 256; permutations[i] = i++) {
        }

        for (int l = 0; l < 256; ++l) {
            int j = random.NextInt(256 - l) + l;
            int k = permutations[l];
            permutations[l] = permutations[j];
            permutations[j] = k;
            permutations[l + 256] = permutations[l];
        }
    }

    double lerp(double p_76311_1_, double p_76311_3_, double p_76311_5_);

    double grad2(int p_76309_1_, double p_76309_2_, double p_76309_4_);

    double grad(int p_76310_1_, double p_76310_2_, double p_76310_4_, double p_76310_6_);
    /**
     * noiseArray should be xSize*ySize*zSize in size
     */
    void populateNoiseArray(std::vector<double>& noiseArray, double xOffset, double yOffset, double zOffset, int xSize, int ySize, int zSize, double xScale, double yScale, double zScale, double noiseScale);
};

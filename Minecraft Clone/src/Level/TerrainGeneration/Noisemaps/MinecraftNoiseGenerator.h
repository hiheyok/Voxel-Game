#pragma once
#include "../Random/JavaRandom.h"
//directly copied from mc source code


class NoiseGeneratorImproved {
private:
    double GRAD_X[16] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
    double GRAD_Y[16] = { 1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0 };
    double GRAD_Z[16] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };
    double GRAD_2X[16] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
    double GRAD_2Z[16] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };

    int* permutations;

public:
    double xCoord;
    double yCoord;
    double zCoord;
    
    ~NoiseGeneratorImproved() {
        if (permutations != nullptr) {
         //   delete[] permutations;
        }
    }

    NoiseGeneratorImproved(JavaRandom& random) {

        permutations = new int[512] {};
        xCoord = random.nextDouble() * 256.0;
        yCoord = random.nextDouble() * 256.0;
        zCoord = random.nextDouble() * 256.0;

        for (int i = 0; i < 256; permutations[i] = i++) {
        }

        for (int l = 0; l < 256; ++l) {
            int j = random.nextInt(256 - l) + l;
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

#pragma once
#include <cmath>
#include <vector>

#include "Level/TerrainGeneration/Random/JavaRandom.h"

// copy paste mc src ported to c++

class NoiseGeneratorSimplex {
   private:
    std::vector<int> p;

    int fastFloor(double value);

    double dot(const int p_151604_0_[], double p_151604_1_, double p_151604_3_);

    double SQRT_3 = sqrt(3.0);

    double F2 = 0.5 * (SQRT_3 - 1.0);
    double G2 = (3.0 - SQRT_3) / 6.0;

   public:
    double xo;
    double yo;
    double zo;

    void Initialize(JavaRandom& seed) {
        p.resize(512);
        xo = seed.NextDouble() * 256.0;
        yo = seed.NextDouble() * 256.0;
        zo = seed.NextDouble() * 256.0;

        for (int i = 0; i < 256; p[i] = i++) {
            ;
        }

        for (int l = 0; l < 256; ++l) {
            int j = seed.NextInt(256 - l) + l;
            int k = p[l];
            p[l] = p[j];
            p[j] = k;
            p[l + 256] = p[l];
        }
    }

    ~NoiseGeneratorSimplex() {}

    NoiseGeneratorSimplex(JavaRandom& seed) { Initialize(seed); }

    NoiseGeneratorSimplex() {
        JavaRandom random;
        Initialize(random);
    }

    double getValue(double p_151605_1_, double p_151605_3_);

    void add(std::vector<double>& p_151606_1_, double p_151606_2_,
             double p_151606_4_, int p_151606_6_, int p_151606_7_,
             double p_151606_8_, double p_151606_10_, double p_151606_12_);
};

#include "Level/TerrainGeneration/Noisemaps/MinecraftNoiseGenerator.h"

static constexpr double GRAD_X[16] = {1.0, -1.0, 1.0,  -1.0, 1.0, -1.0,
                                      1.0, -1.0, 0.0,  0.0,  0.0, 0.0,
                                      1.0, 0.0,  -1.0, 0.0};
static constexpr double GRAD_Y[16] = {1.0, 1.0,  -1.0, -1.0, 0.0, 0.0,
                                      0.0, 0.0,  1.0,  -1.0, 1.0, -1.0,
                                      1.0, -1.0, 1.0,  -1.0};
static constexpr double GRAD_Z[16] = {0.0,  0.0,  0.0, 0.0, 1.0,  1.0,
                                      -1.0, -1.0, 1.0, 1.0, -1.0, -1.0,
                                      0.0,  1.0,  0.0, -1.0};
static constexpr double GRAD_2X[16] = {1.0, -1.0, 1.0,  -1.0, 1.0, -1.0,
                                       1.0, -1.0, 0.0,  0.0,  0.0, 0.0,
                                       1.0, 0.0,  -1.0, 0.0};
static constexpr double GRAD_2Z[16] = {0.0,  0.0,  0.0, 0.0, 1.0,  1.0,
                                       -1.0, -1.0, 1.0, 1.0, -1.0, -1.0,
                                       0.0,  1.0,  0.0, -1.0};

NoiseGeneratorImproved::NoiseGeneratorImproved(JavaRandom& random) {
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

NoiseGeneratorImproved::~NoiseGeneratorImproved() noexcept = default;

double NoiseGeneratorImproved::lerp(double p_76311_1_, double p_76311_3_,
                                    double p_76311_5_) const noexcept {
  return p_76311_3_ + p_76311_1_ * (p_76311_5_ - p_76311_3_);
}

double NoiseGeneratorImproved::grad2(int p_76309_1_, double p_76309_2_,
                                     double p_76309_4_) const noexcept {
  int i = p_76309_1_ & 15;
  return GRAD_2X[i] * p_76309_2_ + GRAD_2Z[i] * p_76309_4_;
}

double NoiseGeneratorImproved::grad(int p_76310_1_, double p_76310_2_,
                                    double p_76310_4_,
                                    double p_76310_6_) const noexcept {
  int i = p_76310_1_ & 15;
  return GRAD_X[i] * p_76310_2_ + GRAD_Y[i] * p_76310_4_ +
         GRAD_Z[i] * p_76310_6_;
}

/**
 * noiseArray should be xSize*ySize*zSize in size
 */
void NoiseGeneratorImproved::populateNoiseArray(
    std::vector<double>& noiseArray, double xOffset, double yOffset,
    double zOffset, int xSize, int ySize, int zSize, double xScale,
    double yScale, double zScale, double noiseScale) const noexcept {
  if (ySize == 1) {
    int i5 = 0;
    int j5 = 0;
    int j = 0;
    int k5 = 0;
    double d14 = 0.0;
    double d15 = 0.0;
    int l5 = 0;
    double d16 = 1.0 / noiseScale;

    for (int j2 = 0; j2 < xSize; ++j2) {
      double d17 = xOffset + static_cast<double>(j2) * xScale + xCoord;
      int i6 = static_cast<int>(d17);

      if (d17 < static_cast<double>(i6)) {
        --i6;
      }

      int k2 = i6 & 255;
      d17 = d17 - static_cast<double>(i6);
      double d18 = d17 * d17 * d17 * (d17 * (d17 * 6.0 - 15.0) + 10.0);

      for (int j6 = 0; j6 < zSize; ++j6) {
        double d19 = zOffset + static_cast<double>(j6) * zScale + zCoord;
        int k6 = static_cast<int>(d19);

        if (d19 < static_cast<double>(k6)) {
          --k6;
        }

        int l6 = k6 & 255;
        d19 = d19 - static_cast<double>(k6);
        double d20 = d19 * d19 * d19 * (d19 * (d19 * 6.0 - 15.0) + 10.0);
        i5 = permutations[k2] + 0;
        j5 = permutations[i5] + l6;
        j = permutations[k2 + 1] + 0;
        k5 = permutations[j] + l6;
        d14 = lerp(d18, grad2(permutations[j5], d17, d19),
                   grad(permutations[k5], d17 - 1.0, 0.0, d19));
        d15 = lerp(d18, grad(permutations[j5 + 1], d17, 0.0, d19 - 1.0),
                   grad(permutations[k5 + 1], d17 - 1.0, 0.0, d19 - 1.0));
        double d21 = lerp(d20, d14, d15);
        int i7 = l5++;
        noiseArray[i7] += d21 * d16;
      }
    }
  } else {
    int i = 0;
    double d0 = 1.0 / noiseScale;
    int k = -1;
    int l = 0;
    int i1 = 0;
    int j1 = 0;
    int k1 = 0;
    int l1 = 0;
    int i2 = 0;
    double d1 = 0.0;
    double d2 = 0.0;
    double d3 = 0.0;
    double d4 = 0.0;

    for (int l2 = 0; l2 < xSize; ++l2) {
      double d5 = xOffset + static_cast<double>(l2) * xScale + xCoord;
      int i3 = static_cast<int>(d5);

      if (d5 < static_cast<double>(i3)) {
        --i3;
      }

      int j3 = i3 & 255;
      d5 = d5 - i3;
      double d6 = d5 * d5 * d5 * (d5 * (d5 * 6.0 - 15.0) + 10.0);

      for (int k3 = 0; k3 < zSize; ++k3) {
        double d7 = zOffset + static_cast<double>(k3) * zScale + zCoord;
        int l3 = static_cast<int>(d7);

        if (d7 < static_cast<double>(l3)) {
          --l3;
        }

        int i4 = l3 & 255;
        d7 = d7 - static_cast<double>(l3);
        double d8 = d7 * d7 * d7 * (d7 * (d7 * 6.0 - 15.0) + 10.0);

        for (int j4 = 0; j4 < ySize; ++j4) {
          double d9 = yOffset + static_cast<double>(j4) * yScale + yCoord;
          int k4 = static_cast<int>(d9);

          if (d9 < static_cast<double>(k4)) {
            --k4;
          }

          int l4 = k4 & 255;
          d9 = d9 - static_cast<double>(k4);
          double d10 = d9 * d9 * d9 * (d9 * (d9 * 6.0 - 15.0) + 10.0);

          if (j4 == 0 || l4 != k) {
            k = l4;
            l = permutations[j3] + l4;
            i1 = permutations[l] + i4;
            j1 = permutations[l + 1] + i4;
            k1 = permutations[j3 + 1] + l4;
            l1 = permutations[k1] + i4;
            i2 = permutations[k1 + 1] + i4;
            d1 = lerp(d6, grad(permutations[i1], d5, d9, d7),
                      grad(permutations[l1], d5 - 1.0, d9, d7));
            d2 = lerp(d6, grad(permutations[j1], d5, d9 - 1.0, d7),
                      grad(permutations[i2], d5 - 1.0, d9 - 1.0, d7));
            d3 = lerp(d6, grad(permutations[i1 + 1], d5, d9, d7 - 1.0),
                      grad(permutations[l1 + 1], d5 - 1.0, d9, d7 - 1.0));
            d4 = lerp(d6, grad(permutations[j1 + 1], d5, d9 - 1.0, d7 - 1.0),
                      grad(permutations[i2 + 1], d5 - 1.0, d9 - 1.0, d7 - 1.0));
          }

          double d11 = lerp(d10, d1, d2);
          double d12 = lerp(d10, d3, d4);
          double d13 = lerp(d8, d11, d12);
          int j7 = i++;
          noiseArray[j7] += d13 * d0;
        }
      }
    }
  }
}

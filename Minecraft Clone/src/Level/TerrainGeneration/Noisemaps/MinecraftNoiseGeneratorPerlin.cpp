#include "Level/TerrainGeneration/Noisemaps/MinecraftNoiseGeneratorPerlin.h"

NoiseGeneratorPerlin::NoiseGeneratorPerlin(JavaRandom seed, int levelsIn) {
  levels = levelsIn;
  noiseLevels = std::vector<NoiseGeneratorSimplex>(levelsIn);

  for (int i = 0; i < levelsIn; ++i) {
    noiseLevels[i] = NoiseGeneratorSimplex(seed);
  }
}

NoiseGeneratorPerlin::~NoiseGeneratorPerlin() = default;

double NoiseGeneratorPerlin::getValue(double p_151601_1_, double p_151601_3_) {
  double d0 = 0.0;
  double d1 = 1.0;

  for (int i = 0; i < levels; ++i) {
    d0 += noiseLevels[i].getValue(p_151601_1_ * d1, p_151601_3_ * d1) / d1;
    d1 /= 2.0;
  }

  return d0;
}

std::vector<double> NoiseGeneratorPerlin::getRegion(
    std::vector<double>& p_151599_1_, double p_151599_2_, double p_151599_4_,
    int p_151599_6_, int p_151599_7_, double p_151599_8_, double p_151599_10_,
    double p_151599_12_) {
  return getRegion(p_151599_1_, p_151599_2_, p_151599_4_, p_151599_6_,
                   p_151599_7_, p_151599_8_, p_151599_10_, p_151599_12_, 0.5);
}

std::vector<double> NoiseGeneratorPerlin::getRegion(
    std::vector<double>& p_151600_1_, double p_151600_2_, double p_151600_4_,
    int p_151600_6_, int p_151600_7_, double p_151600_8_, double p_151600_10_,
    double p_151600_12_, double p_151600_14_) {
  if ((p_151600_1_.size() == 0) &&
      p_151600_1_.size() >= p_151600_6_ * p_151600_7_) {
    for (int i = 0; i < p_151600_1_.size(); ++i) {
      p_151600_1_[i] = 0.0;
    }
  } else {
    p_151600_1_ = std::vector<double>(p_151600_6_ * p_151600_7_);
  }

  double d1 = 1.0;
  double d0 = 1.0;

  for (int j = 0; j < levels; ++j) {
    noiseLevels[j].add(p_151600_1_, p_151600_2_, p_151600_4_, p_151600_6_,
                       p_151600_7_, p_151600_8_ * d0 * d1,
                       p_151600_10_ * d0 * d1, 0.55 / d1);
    d0 *= p_151600_12_;
    d1 *= p_151600_14_;
  }

  return p_151600_1_;
}

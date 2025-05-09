#include "MinecraftNoiseGeneratorSimplex.h"

static constexpr int grad3[12][3]{
    {1, 1, 0},  {-1, 1, 0},  {1, -1, 0}, {-1, -1, 0}, {1, 0, 1},  {-1, 0, 1},
    {1, 0, -1}, {-1, 0, -1}, {0, 1, 1},  {0, -1, 1},  {0, 1, -1}, {0, -1, -1}};

NoiseGeneratorSimplex::NoiseGeneratorSimplex(JavaRandom& seed) {
  Initialize(seed);
}
NoiseGeneratorSimplex::~NoiseGeneratorSimplex() {}

NoiseGeneratorSimplex::NoiseGeneratorSimplex() {
  JavaRandom random;
  Initialize(random);
}

void NoiseGeneratorSimplex::Initialize(JavaRandom& seed) {
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

int NoiseGeneratorSimplex::fastFloor(double value) const noexcept {
  return static_cast<int>(value) - 1 * (value <= 0.0);
}

double NoiseGeneratorSimplex::dot(const int p_151604_0_[], double p_151604_1_,
                                  double p_151604_3_) const noexcept {
  return static_cast<double>(p_151604_0_[0]) * p_151604_1_ +
         static_cast<double>(p_151604_0_[1]) * p_151604_3_;
}

double NoiseGeneratorSimplex::getValue(double p_151605_1_, double p_151605_3_) {
  double d3 = 0.5 * (SQRT_3 - 1.0);
  double d4 = (p_151605_1_ + p_151605_3_) * d3;
  int i = fastFloor(p_151605_1_ + d4);
  int j = fastFloor(p_151605_3_ + d4);
  double d5 = (3.0 - SQRT_3) / 6.0;
  double d6 = static_cast<double>(i + j) * d5;
  double d7 = static_cast<double>(i) - d6;
  double d8 = static_cast<double>(j) - d6;
  double d9 = p_151605_1_ - d7;
  double d10 = p_151605_3_ - d8;
  int k;
  int l;

  if (d9 > d10) {
    k = 1;
    l = 0;
  } else {
    k = 0;
    l = 1;
  }

  double d11 = d9 - static_cast<double>(k) + d5;
  double d12 = d10 - static_cast<double>(l) + d5;
  double d13 = d9 - 1.0 + 2.0 * d5;
  double d14 = d10 - 1.0 + 2.0 * d5;
  int i1 = i & 255;
  int j1 = j & 255;
  int k1 = p[i1 + p[j1]] % 12;
  int l1 = p[i1 + k + p[j1 + l]] % 12;
  int i2 = p[i1 + 1 + p[j1 + 1]] % 12;
  double d15 = 0.5 - d9 * d9 - d10 * d10;
  double d0;

  if (d15 < 0.0) {
    d0 = 0.0;
  } else {
    d15 = d15 * d15;
    d0 = d15 * d15 * dot(grad3[k1], d9, d10);
  }

  double d16 = 0.5 - d11 * d11 - d12 * d12;
  double d1;

  if (d16 < 0.0) {
    d1 = 0.0;
  } else {
    d16 = d16 * d16;
    d1 = d16 * d16 * dot(grad3[l1], d11, d12);
  }

  double d17 = 0.5 - d13 * d13 - d14 * d14;
  double d2;

  if (d17 < 0.0) {
    d2 = 0.0;
  } else {
    d17 = d17 * d17;
    d2 = d17 * d17 * dot(grad3[i2], d13, d14);
  }

  return 70.0 * (d0 + d1 + d2);
}

void NoiseGeneratorSimplex::add(std::vector<double>& p_151606_1_,
                                double p_151606_2_, double p_151606_4_,
                                int p_151606_6_, int p_151606_7_,
                                double p_151606_8_, double p_151606_10_,
                                double p_151606_12_) {
  int i = 0;

  for (int j = 0; j < p_151606_7_; ++j) {
    double d0 = (p_151606_4_ + static_cast<double>(j)) * p_151606_10_ + yo;
    for (int k = 0; k < p_151606_6_; ++k) {
      double d1 = (p_151606_2_ + static_cast<double>(k)) * p_151606_8_ + xo;
      double d5 = (d1 + d0) * F2;
      int l = fastFloor(d1 + d5);
      int i1 = fastFloor(d0 + d5);
      double d6 = static_cast<double>(l + i1) * G2;
      double d7 = static_cast<double>(l) - d6;
      double d8 = static_cast<double>(i1) - d6;
      double d9 = d1 - d7;
      double d10 = d0 - d8;
      int j1;
      int k1;

      if (d9 > d10) {
        j1 = 1;
        k1 = 0;
      } else {
        j1 = 0;
        k1 = 1;
      }

      double d11 = d9 - static_cast<double>(j1) + G2;
      double d12 = d10 - static_cast<double>(k1) + G2;
      double d13 = d9 - 1.0 + 2.0 * G2;
      double d14 = d10 - 1.0 + 2.0 * G2;
      int l1 = l & 255;
      int i2 = i1 & 255;
      int j2 = p[l1 + p[i2]] % 12;
      int k2 = p[l1 + j1 + p[i2 + k1]] % 12;
      int l2 = p[l1 + 1 + p[i2 + 1]] % 12;
      double d15 = 0.5 - d9 * d9 - d10 * d10;
      double d2;

      if (d15 < 0.0) {
        d2 = 0.0;
      } else {
        d15 = d15 * d15;
        d2 = d15 * d15 * dot(grad3[j2], d9, d10);
      }

      double d16 = 0.5 - d11 * d11 - d12 * d12;
      double d3;

      if (d16 < 0.0) {
        d3 = 0.0;
      } else {
        d16 = d16 * d16;
        d3 = d16 * d16 * dot(grad3[k2], d11, d12);
      }

      double d17 = 0.5 - d13 * d13 - d14 * d14;
      double d4;

      if (d17 < 0.0) {
        d4 = 0.0;
      } else {
        d17 = d17 * d17;
        d4 = d17 * d17 * dot(grad3[l2], d13, d14);
      }

      int i3 = i++;
      p_151606_1_[i3] += 70.0 * (d2 + d3 + d4) * p_151606_12_;
    }
  }
}

#pragma once
#include <cmath>
#include <vector>

#include "Level/TerrainGeneration/Random/JavaRandom.h"

// copy paste mc src ported to c++

class NoiseGeneratorSimplex {
 private:
  std::vector<int> p;

  int fastFloor(double value) const noexcept;

  double dot(const int p_151604_0_[], double p_151604_1_,
             double p_151604_3_) const noexcept;

  double SQRT_3 = sqrt(3.0);

  double F2 = 0.5 * (SQRT_3 - 1.0);
  double G2 = (3.0 - SQRT_3) / 6.0;

 public:
  explicit NoiseGeneratorSimplex(JavaRandom& seed);
  ~NoiseGeneratorSimplex();

  NoiseGeneratorSimplex();

  void Initialize(JavaRandom& seed);

  double getValue(double p_151605_1_, double p_151605_3_);

  void add(std::vector<double>& p_151606_1_, double p_151606_2_,
           double p_151606_4_, int p_151606_6_, int p_151606_7_,
           double p_151606_8_, double p_151606_10_, double p_151606_12_);

  double xo;
  double yo;
  double zo;
};

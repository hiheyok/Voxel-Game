#pragma once
#include "Level/TerrainGeneration/Random/JavaRandom.h"
// directly copied from mc source code

class NoiseGeneratorImproved {
 public:
  explicit NoiseGeneratorImproved(JavaRandom& random);

  ~NoiseGeneratorImproved() noexcept;

  double lerp(double p_76311_1_, double p_76311_3_,
              double p_76311_5_) const noexcept;

  double grad2(int p_76309_1_, double p_76309_2_,
               double p_76309_4_) const noexcept;

  double grad(int p_76310_1_, double p_76310_2_, double p_76310_4_,
              double p_76310_6_) const noexcept;
  /**
   * noiseArray should be xSize*ySize*zSize in size
   */
  void populateNoiseArray(std::vector<double>& noiseArray, double xOffset,
                          double yOffset, double zOffset, int xSize, int ySize,
                          int zSize, double xScale, double yScale,
                          double zScale, double noiseScale) const noexcept;
  double xCoord;
  double yCoord;
  double zCoord;

 private:
  std::vector<int> permutations;
};

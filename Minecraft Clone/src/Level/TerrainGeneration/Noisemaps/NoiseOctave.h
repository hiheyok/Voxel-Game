#pragma once
#include <vector>
#include "Noise.h"
#include "MinecraftNoiseGenerator.h"

class NoiseOctave {

private:

	static int64_t lfloor(double value);

	int octaves = NULL;
	std::vector<NoiseGeneratorImproved> generatorCollection;
public:
	NoiseOctave(JavaRandom& seed, int octavesIn) {
		octaves = octavesIn;
		
		for (int i = 0; i < octavesIn; i++) {
			generatorCollection.emplace_back(seed);
		}
	}

	NoiseOctave() {

	}

	std::vector<double> generateNoiseOctaves(std::vector<double>& noiseArray, int xOffset, int yOffset, int zOffset, int xSize, int ySize, int zSize, double xScale, double yScale, double zScale);

	std::vector<double> generateNoiseOctaves(std::vector<double>& noiseArray, int xOffset, int zOffset, int xSize, int zSize, double xScale, double zScale, double p_76305_10_);


};
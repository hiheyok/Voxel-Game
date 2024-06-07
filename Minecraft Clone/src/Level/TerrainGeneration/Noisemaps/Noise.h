#pragma once
#include "../../../Utils/FastNoiseLite.h"
#include "../Random/JavaRandom.h"

class NoiseGenerator {
private:
	FastNoiseLite noise;

public:
	NoiseGenerator(JavaRandom seed) {}
};
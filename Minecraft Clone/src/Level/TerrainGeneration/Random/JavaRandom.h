#pragma once
#include <random>
#include <exception>

class JavaRandom {
private:
	bool haveNextNextGaussian;
	double nextNextGaussian;
	int64_t rseed;
	int64_t serialVersionUID = 3905348978240129619L;

public:
	JavaRandom() {
		JavaRandom(time(NULL));
	}

	JavaRandom(uint64_t seed) {
		setSeed(seed);
	}

	void setSeed(int64_t seed);

	int next(int bits);

	int nextInt(int n);

	int nextInt();

	uint64_t nextLong();

	bool nextBoolean();

	float nextFloat();

	double nextDouble();
};
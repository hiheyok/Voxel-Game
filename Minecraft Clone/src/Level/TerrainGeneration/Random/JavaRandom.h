#pragma once
#include <random>
#include <exception>

class JavaRandom {
private:
	bool have_next_next_gaussian_;
	double next_next_gaussian;
	int64_t rseed_;
	int64_t serial_version_uid_ = 3905348978240129619L;

public:
	JavaRandom() : JavaRandom{time(NULL)} {
	}

	JavaRandom(uint64_t seed) {
		SetSeed(seed);
	}

	void SetSeed(int64_t seed);

	int Next(int bits);

	int NextInt(int n);

	int NextInt();

	uint64_t NextLong();

	bool NextBoolean();

	float NextFloat();

	double NextDouble();
};
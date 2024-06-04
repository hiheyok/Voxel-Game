#pragma once
#include <cmath>

struct Guassian {
	float StandardDeviation = 1.f;
	float Mean = 0.f;

	Guassian(float SD, float M) : StandardDeviation(SD), Mean(M) {

	}

	float GetProbabilityLeft(float x) {
		float z = (x - Mean) / StandardDeviation;
		return 0.5f * (1.f + std::erf(z / sqrt(2)));
	}

	void GetProbabilityRight() {

	}
};
#include "Probability.h"
#include <cstdlib>


bool TestProbability(double odds) {
	int rng = rand();
	int maxRNG = RAND_MAX;

	if (((double)RAND_MAX) * odds >= (double)rng) {
		return true;
	}

	return false;
	
}
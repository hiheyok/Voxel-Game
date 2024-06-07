#include "JavaRandom.h"

void JavaRandom::setSeed(int64_t seed) {
	rseed = (seed ^ 0x5DEECE66DLL) & ((1LL << 48) - 1);
	haveNextNextGaussian = false;
}

int JavaRandom::next(int bits) {
	rseed = (rseed * 0x5DEECE66DLL + 0xBLL) & ((1LL << 48) - 1);
	return (int)(rseed >> (48 - bits));
}

int JavaRandom::nextInt(int n) {
	if (n <= 0)
		throw std::exception("n must be positive");
	if ((n & (-n)) == n) //n is power of 2
		return (int)((n * (int64_t)next(31)) >> 31);
	int bits, val;
	do
	{
		bits = next(31);
		val = bits % n;
	} while (bits - val + (n - 1) < 0);
	return val;
}

int JavaRandom::nextInt() {
	return next(32);
}

uint64_t JavaRandom::nextLong() {
	return ((long)next(32) << 32) + next(32);
}

bool JavaRandom::nextBoolean() {
	return next(1) != 0;
}

float JavaRandom::nextFloat() {
	return next(24) / (float)(1 << 24);
}

double JavaRandom::nextDouble() {
	return (((int64_t)next(26) << 27) + next(27)) / (double)(1LL << 53);
}
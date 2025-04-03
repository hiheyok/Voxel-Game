#include "Level/TerrainGeneration/Random/JavaRandom.h"
#include "Utils/LogUtils.h"

void JavaRandom::SetSeed(int64_t seed) {
    rseed_ = (seed ^ 0x5DEECE66DLL) & ((1LL << 48) - 1);
    have_next_next_gaussian_ = false;
}

int JavaRandom::Next(int bits) {
    rseed_ = (rseed_ * 0x5DEECE66DLL + 0xBLL) & ((1LL << 48) - 1);
    return (int)(rseed_ >> (48 - bits));
}

int JavaRandom::NextInt(int n) {
    if (n <= 0)
        g_logger.LogError("JavaRandom", "n must be positive");
    if ((n & (-n)) == n) //n is power of 2
        return (int)((n * (int64_t)Next(31)) >> 31);
    int bits, val;
    do
    {
        bits = Next(31);
        val = bits % n;
    } while (bits - val + (n - 1) < 0);
    return val;
}

int JavaRandom::NextInt() {
    return Next(32);
}

uint64_t JavaRandom::NextLong() {
    return ((long long)Next(32) << 32) + Next(32);
}

bool JavaRandom::NextBoolean() {
    return Next(1) != 0;
}

float JavaRandom::NextFloat() {
    return Next(24) / (float)(1 << 24);
}

double JavaRandom::NextDouble() {
    return (((int64_t)Next(26) << 27) + Next(27)) / (double)(1LL << 53);
}
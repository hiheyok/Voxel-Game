#include "NoiseOctave.h"

using namespace std;

int64_t NoiseOctave::lfloor(double value) {
    int64_t i = (int64_t)value;
    return value < (double)i ? i - 1L : i;
}

vector<double> NoiseOctave::generateNoiseOctaves(vector<double>& noiseArray, int xOffset, int yOffset, int zOffset, int xSize, int ySize, int zSize, double xScale, double yScale, double zScale) {
    if (noiseArray.size() == 0) {
        noiseArray.resize(xSize * ySize * zSize);
    }
    else {
        for (int i = 0; i < noiseArray.size(); ++i)
        {
            noiseArray[i] = 0.0;
        }
    }

    double d3 = 1.0;

    for (int j = 0; j < octaves; ++j)
    {
        double d0 = (double)xOffset * d3 * xScale;
        double d1 = (double)yOffset * d3 * yScale;
        double d2 = (double)zOffset * d3 * zScale;
        int64_t k = lfloor(d0);
        int64_t l = lfloor(d2);
        d0 = d0 - (double)k;
        d2 = d2 - (double)l;
        k = k % 16777216LL;
        l = l % 16777216LL;
        d0 = d0 + (double)k;
        d2 = d2 + (double)l;
        generatorCollection[j].populateNoiseArray(noiseArray, d0, d1, d2, xSize, ySize, zSize, xScale * d3, yScale * d3, zScale * d3, d3);

        d3 /= 2.0;
    }

    return noiseArray;

}

vector<double> NoiseOctave::generateNoiseOctaves(vector<double>& noiseArray, int xOffset, int zOffset, int xSize, int zSize, double xScale, double zScale, double p_76305_10_) {
    return generateNoiseOctaves(noiseArray, xOffset, 10, zOffset, xSize, 1, zSize, xScale, 1.0, zScale);
}
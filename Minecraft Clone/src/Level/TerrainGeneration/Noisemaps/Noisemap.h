#pragma once
#include "../../../Utils/FastNoiseLite.h"

class NoiseMapTypes {
public:
	void Initialize(int32_t SEED) {
		IslandLayer4096.SetNoiseType(IslandLayer4096.NoiseType_OpenSimplex2);
		IslandLayer4096.SetFrequency(1.f / 256.f);
		IslandLayer4096.SetSeed(SEED);

		IslandLayer2048.SetNoiseType(IslandLayer4096.NoiseType_OpenSimplex2);
		IslandLayer2048.SetFrequency(1.f / 128.f);
		IslandLayer2048.SetSeed(SEED + SEED_OFFSET);

		TemperatureLayer512.SetNoiseType(TemperatureLayer512.NoiseType_OpenSimplex2);
		TemperatureLayer512.SetFrequency(1.f / 32.f);
		TemperatureLayer512.SetSeed(SEED + SEED_OFFSET * 3);

		RainfallLayer512.SetNoiseType(RainfallLayer512.NoiseType_OpenSimplex2);
		RainfallLayer512.SetFrequency(1.f / 32.f);
		RainfallLayer512.SetSeed(SEED + SEED_OFFSET * 5);

		OctaveNoise.SetNoiseType(OctaveNoise.NoiseType_OpenSimplex2);
		OctaveNoise.SetFrequency(1.f);
		OctaveNoise.SetSeed(SEED + SEED_OFFSET * 5);

		double Half = 1.0;
		double Two = 1;

		for (int i = 0; i < 32; i++) {
			PowTableHalf[i] = Half;
			PowTable[i] = Two;
			Half = Half * 0.5;
			Two = Two * 2.0;
		}
	}

	template <typename T> float GetNoise(T x, T y, T z, int Octaves, float Zoom) {
		float gx = static_cast<float>(x);
		float gy = static_cast<float>(y);
		float gz = static_cast<float>(z);

		float out = 0.0f;

		for (int i = 0; i < Octaves; i++) {
			float n = (OctaveNoise.GetNoise(gx * PowTable[i], gy * PowTable[i], gz * PowTable[i]) + 1) * 0.5f;
			out += n * PowTableHalf[i];
		}

		out = out * ((-0.5f) / (PowTableHalf[Octaves] - 1));

		return out;
	}

	FastNoiseLite IslandLayer4096;
	FastNoiseLite IslandLayer2048;
	FastNoiseLite TemperatureLayer512;
	FastNoiseLite RainfallLayer512;
private:

	float PowTableHalf[32]{};
	float PowTable[32]{};

	FastNoiseLite OctaveNoise;

	int32_t SEED_OFFSET = 0x93496;
};

class NoiseMaps1122 {
public:
	FastNoiseLite minLimitPerlinNoise;
	FastNoiseLite maxLimitPerlinNoise;
	FastNoiseLite mainPerlinNoise;
	FastNoiseLite surfaceNoise;
	FastNoiseLite scaleNoise;
	FastNoiseLite depthNoise;
	FastNoiseLite forestNoise;
};
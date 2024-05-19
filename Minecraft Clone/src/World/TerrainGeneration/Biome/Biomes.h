#include "BiomeMapping.h"

struct BiomeParamters {
	BiomeParamters(float Height, float Flat, float Wet) {
		HeightIntensity = Height;
		Flatness = Flat;
		Wetness = Wet;
	}

	float HeightIntensity = 0.f;
	float Flatness = 0.f;
	float Wetness = 0.f;
};

class Biomes {
public:
	BiomeParamters Tundra = BiomeParamters(0.7f, 0.2f, 0.4f);
	BiomeParamters Plains = BiomeParamters(0.3f, 0.7f, 0.2f);
	BiomeParamters Taiga = BiomeParamters(0.5f, 0.4f, 0.5f);
	BiomeParamters Shrubland = BiomeParamters(0.3f, 0.5f, 0.5f);
	BiomeParamters Forest = BiomeParamters(0.35f, 0.6f, 0.55f);
	BiomeParamters Swamp = BiomeParamters(0.2f, 0.8f, 0.9f);
	BiomeParamters Desert = BiomeParamters(0.1f, 0.7f, 0.1f);
	BiomeParamters Savanna = BiomeParamters(0.3f, 0.6f, 0.3f);
	BiomeParamters SeasonalForest = BiomeParamters(0.5f, 0.4f, 0.5f);
	BiomeParamters RainForest = BiomeParamters(0.8f, 0.2f, 0.99f);

	BiomeParamters GetBiome(BiomeMapping::MAJOR_BIOME_TYPE Biome) {
		switch (Biome) {
		case BiomeMapping::TUNDRA:
			return Tundra;
		case BiomeMapping::PLAINS:
			return Plains;
		case BiomeMapping::TAIGA:
			return Taiga;
		case BiomeMapping::SHRUBLAND:
			return Shrubland;
		case BiomeMapping::FOREST:
			return Forest;
		case BiomeMapping::SWAMP:
			return Swamp;
		case BiomeMapping::DESERT:
			return Desert;
		case BiomeMapping::SAVANNA:
			return Savanna;
		case BiomeMapping::SEASONAL_FOREST:
			return SeasonalForest;
		case BiomeMapping::RAIN_FOREST:
			return RainForest;
		}
	}
};
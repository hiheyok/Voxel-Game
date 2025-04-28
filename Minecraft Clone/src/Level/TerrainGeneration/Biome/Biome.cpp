#include "Level/TerrainGeneration/Biome/Biome.h"

Registry<ResourceLocation, Biome*> Biome::REGISTRY;
FastHashMap<Biome*, int> Biome::BiomeIDs;
FastHashMap<int, Biome*> Biome::MUTATION_TO_BASE_ID_MAP;

NoiseGeneratorPerlin Biome::TEMPERATURE_NOISE{JavaRandom{1234L}, 1};
NoiseGeneratorPerlin Biome::GRASS_COLOR_NOISE{JavaRandom{2345L}, 1};
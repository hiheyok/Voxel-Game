#pragma once
#include <unordered_map>
#include "../../../Core/Registry/Registry.h"
#include "../../../Core/Registry/ResourceLocation.h"
#include "BiomeProperties.h"
#include "../../Chunk/TallChunk.h"
class Biome {
private:
	std::string biomeName = "";
	std::string baseBiome = "";
	float baseHeight = 0.1F;
	float heightVariation = 0.2F;
	float temperature = 0.5F;
	float rainfall = 0.5F;
	int waterColor = 16777215;
	bool enableSnow;
	bool enableRain = true;
public:
	BiomeProperties biomeProperties;

	static Registry<std::string, Biome*> REGISTRY;
	static std::unordered_map<Biome*, int> BiomeIDs;
	static std::unordered_map<int, Biome*> MUTATION_TO_BASE_ID_MAP;
	static NoiseGeneratorPerlin TEMPERATURE_NOISE;
	static NoiseGeneratorPerlin GRASS_COLOR_NOISE;

	BlockID topBlock = Blocks.GRASS;
	BlockID fillerBlock = Blocks.DIRT;

	enum TempCategory {
		OCEAN,
		COLD,
		MEDIUM,
		WARM
	};

	static void Register(int ID, std::string BiomeName, Biome* biome) {
		REGISTRY.Register(ID, ResourceLocation(BiomeName).PATH, biome);
		BiomeIDs[biome] = ID;
		Logger.LogDebug("Biome", "Registered biome: " + BiomeName);
		if (biome->isMutation()) {
			MUTATION_TO_BASE_ID_MAP[ID] = biome;
		}
	}

	bool isMutation() {
		return baseBiome.length() != 0;
	}

	static int getIdForBiome(Biome* biome) {
		if (!BiomeIDs.count(biome)) {
			throw std::exception("Invalid biome");
		}
		return BiomeIDs[biome];
	}

	static Biome* getBiomeForId(int ID) {
		return REGISTRY.GetValue(REGISTRY.getKey(ID));
	}

	static Biome* getBiome(int id) {
		return getBiome(id, (Biome*)nullptr);
	}

	static Biome* getBiome(int biomeId, Biome* fallback) {
		Biome* biome = nullptr;
		try {
			biome = REGISTRY.GetValue(REGISTRY.getKey(biomeId));
		}
		catch (std::exception& e) {
			//do nothing
		}
		
		return biome == nullptr ? fallback : biome;
	}

	static Biome* getMutationForBiome(Biome* biome) {
		if (!Biome::MUTATION_TO_BASE_ID_MAP.count(getIdForBiome(biome))) {
			return nullptr;
		}

		return Biome::MUTATION_TO_BASE_ID_MAP[getIdForBiome(biome)];
	}

	virtual Biome::TempCategory getTempCategory() {
		if ((double)getDefaultTemperature() < 0.2)
		{
			return Biome::TempCategory::COLD;
		}
		else
		{
			return (double)getDefaultTemperature() < 1.0 ? Biome::TempCategory::MEDIUM : Biome::TempCategory::WARM;
		}
	}

	float getDefaultTemperature() {
		return temperature;
	}

	bool isSnowyBiome() {
		return enableSnow;
	}

	float getHeightVariation() {
		return heightVariation;
	}

	float getBaseHeight() {
		return baseHeight;
	}

	float getTemperature(glm::ivec3 pos)
	{
		if (pos.y > 64)
		{
			float f = (float)(TEMPERATURE_NOISE.getValue((double)((float)pos.x / 8.0F), (double)((float)pos.z / 8.0F)) * 4.0);
			return getDefaultTemperature() - (f + (float)pos.y - 64.0F) * 0.05F / 30.0F;
		}
		else
		{
			return getDefaultTemperature();
		}
	}

	Biome() {

	}

	Biome(BiomeProperties p) {
		biomeProperties = p;
		biomeName = p.biomeName;
		baseBiome = p.baseBiome;
		baseHeight = p.baseHeight;
		heightVariation = p.heightVariation;
		temperature = p.temperature;
		rainfall = p.rainfall;
		waterColor = p.waterColor;
		enableSnow = p.enableSnow;
		enableRain = p.enableRain;
	}

	virtual ~Biome() {

	}

	virtual const std::type_info& getBiomeClass() const {
		return typeid(*this);
	}

	virtual void genTerrainBlocks(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) {
		generateBiomeTerrain(rand, chunk, x, z, noiseVal, settings);
	}

	void SetBlockChunkSafe(TallChunk* chunk, BlockID block, int x, int y, int z) {
		chunk->SetBlockUnsafe(x, y, z, block);
	}

	BlockID GetBlockChunkSafe(TallChunk* chunk, int x, int y, int z) {
		chunk->GetBlockUnsafe( x, y, z);
	}

	void generateBiomeTerrain(JavaRandom& rand, TallChunk* chunk, int x, int z, double noiseVal, ChunkGeneratorSettings* settings) {

		int i = settings->seaLevel;
		BlockID iblockstate = topBlock;
		BlockID iblockstate1 = fillerBlock;
		int j = -1;
		int k = (int)(noiseVal / 3.0 + 3.0 + rand.nextDouble() * 0.25);
		int l = x & 15;
		int i1 = z & 15;

		for (int j1 = 255; j1 >= 0; --j1)
		{
			if (j1 <= rand.nextInt(5))
			{
				chunk->SetBlockUnsafe(i1, j1, l, Blocks.BEDROCK);
			}
			else
			{
				BlockID iblockstate2 = chunk->GetBlockUnsafe(i1, j1, l);

				if (iblockstate2 == Blocks.AIR)
				{
					j = -1;
				}
				else if (iblockstate2 == Blocks.STONE)
				{
					if (j == -1)
					{
						if (k <= 0)
						{
							iblockstate = Blocks.AIR;
							iblockstate1 = Blocks.STONE;
						}
						else if (j1 >= i - 4 && j1 <= i + 1)
						{
							iblockstate = topBlock;
							iblockstate1 = fillerBlock;
						}

						if (j1 < i && (iblockstate == Blocks.AIR))
						{
							if (getTemperature(glm::ivec3(x, j1, z)) < 0.15F)
							{
								iblockstate = Blocks.ICE;
							}
							else
							{
								iblockstate = Blocks.WATER;
							}
						}

						j = k;

						if (j1 >= i - 1)
						{
							chunk->SetBlockUnsafe(i1, j1, l, iblockstate);
						}
						else if (j1 < i - 7 - k)
						{
							iblockstate = Blocks.AIR;
							iblockstate1 = Blocks.STONE;
							chunk->SetBlockUnsafe(i1, j1, l, Blocks.GRAVEL);
						}
						else
						{
							chunk->SetBlockUnsafe(i1, j1, l, iblockstate1);
						}
					}
					else if (j > 0)
					{
						--j;
						chunk->SetBlockUnsafe(i1, j1, l, iblockstate1);

						if (j == 0 && iblockstate1 == Blocks.SAND && k > 1)
						{
							j = rand.nextInt(4) + std::max(0, j1 - 63);
							iblockstate1 = Blocks.RED_SAND;
						}
					}
				}
			}
		}
	}

};

_declspec(selectany) Registry<std::string, Biome*> Biome::REGISTRY = Registry<std::string, Biome*>();
_declspec(selectany) std::unordered_map<Biome*, int> Biome::BiomeIDs = std::unordered_map<Biome*, int>();
_declspec(selectany) std::unordered_map<int, Biome*> Biome::MUTATION_TO_BASE_ID_MAP = std::unordered_map<int, Biome*>();

_declspec(selectany) NoiseGeneratorPerlin Biome::TEMPERATURE_NOISE = NoiseGeneratorPerlin(JavaRandom(1234L), 1);
_declspec(selectany) NoiseGeneratorPerlin Biome::GRASS_COLOR_NOISE = NoiseGeneratorPerlin(JavaRandom(2345L), 1);
#pragma once
#include "Level/TerrainGeneration/GenLayer/CombineLayers.h"
#include "Core/Typenames.h"
//mc src
inline std::mutex g_biome_lock;

class BiomeProvider {
private:
    static std::vector<GenLayer*> genBiomes;
    static std::vector<GenLayer*> biomeIndexLayer;
    static ChunkGeneratorSettings* settings_;
    static FastHashMap<size_t, size_t> ThreadGenIndex;
    static long long biomeProviderSeed;
public:
    BiomeProvider() {

    }

    BiomeProvider(long long seed, ChunkGeneratorSettings* settingsIn) {
        (void)seed;
        (void)settingsIn;
        g_logger.LogError("BiomeProvider::BiomeProvider", "Don't use this");
    }

    static void init(long long seed, ChunkGeneratorSettings* settingsIn) {
        std::lock_guard<std::mutex> lock{ g_biome_lock };
        std::vector<GenLayer*> agenlayer = CombinedGenLayers::initializeAllBiomeGenerators(seed, settingsIn);
        BiomeProvider::genBiomes.push_back(agenlayer[0]);
        BiomeProvider::biomeIndexLayer.push_back(agenlayer[1]);
        BiomeProvider::settings_ = settingsIn;
        BiomeProvider::biomeProviderSeed = seed;
    }

    static std::vector<Biome*> getBiomesForGeneration(int x, int z, int width, int length) {
        size_t threadHash = std::hash<std::thread::id>{}(std::this_thread::get_id()); //Used to detect if a new thread is running
        size_t mapSize = BiomeProvider::ThreadGenIndex.size();
        if (!ThreadGenIndex.count(threadHash)) {
            BiomeProvider::ThreadGenIndex[threadHash] = mapSize;
            init(biomeProviderSeed, settings_);
        }

        size_t threadIndex = BiomeProvider::ThreadGenIndex[threadHash];

        std::vector<int> aint = BiomeProvider::genBiomes[threadIndex]->getInts(x, z, width, length);

        std::vector<Biome*> list(width * length);

        for (int i = 0; i < width * length; ++i) {
            list[i] = Biome::getBiome(aint[i], Biomes::DEFAULT);
        }
        return list;
    }

    static std::vector<Biome*> getBiomes(int x, int z, int width, int length) {
        size_t threadHash = std::hash<std::thread::id>{}(std::this_thread::get_id()); //Used to detect if a new thread is running
        size_t mapSize = BiomeProvider::ThreadGenIndex.size();
        if (!ThreadGenIndex.count(threadHash)) {
            BiomeProvider::ThreadGenIndex[threadHash] = mapSize;
            init(biomeProviderSeed, settings_);
        }

        size_t threadIndex = BiomeProvider::ThreadGenIndex[threadHash];

        std::vector<int> aint = biomeIndexLayer[threadIndex]->getInts(x, z, width, length);
        std::vector<Biome*> out(width * length);
        for (int i = 0; i < width * length; ++i)
        {
            out[i] = Biome::getBiome(aint[i], Biomes::DEFAULT);
        }

        return out;
    }
};

inline std::vector<GenLayer*> BiomeProvider::genBiomes = std::vector<GenLayer*>();
inline std::vector<GenLayer*> BiomeProvider::biomeIndexLayer = std::vector<GenLayer*>();
inline ChunkGeneratorSettings* BiomeProvider::settings_ = nullptr;
inline long long BiomeProvider::biomeProviderSeed = 0;
inline FastHashMap<size_t, size_t> BiomeProvider::ThreadGenIndex = FastHashMap<size_t, size_t>();

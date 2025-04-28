#include "Level/Level.h"

#include "Level/Dimension/Dimensions/Overworld.h"
#include "Level/TerrainGeneration/Biome/BiomeProvider.h"
#include "Level/TerrainGeneration/Biome/Biomes.h"

Level::Level() = default;
Level::~Level() = default;

void Level::Start(int worldGenThreadCount, int light_engine_thread_count_) {
    DimensionProperties properties;
    main_world_ = std::make_unique<OverworldDimension>(properties);
    Biomes::RegisterBiome();
    BiomeProvider::init(kWorldSeed, new ChunkGeneratorSettings);
}

void Level::Stop() {}

void Level::updateDimensions() { main_world_->Update(); }
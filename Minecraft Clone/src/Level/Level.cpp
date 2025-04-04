#include "Level/Level.h"
#include "Level/LevelLoader.h"
#include "Level/Dimension/Dimensions/Overworld.h"
#include "Level/World/WorldInteraction/WorldLoader.h"

Level::Level() = default;
Level::~Level() = default;

void Level::Start(int worldGenThreadCount, int light_engine_thread_count_) {
    DimensionProperties properties;
    main_world_ = std::make_unique<OverworldDimension>();
    main_world_->Initialize(properties);
    
    long long Seed = 0;//;-501575345763903LL;//-501575345763903LL;//-1587754402LL
    level_loader_ = std::make_unique<LevelLoader>();
    level_loader_->Start(worldGenThreadCount, light_engine_thread_count_, main_world_->world_interactions_.worldLoader_->GetWorld(), Seed);
}

void Level::Stop() {
    level_loader_->Stop();
}

void Level::updateDimensions() {
    //Generated Chunks
    level_loader_->SendRequestedChunks(main_world_->world_interactions_.worldLoader_->GetRequestedChunks(), main_world_->GetGeneratorType());
    std::vector<std::unique_ptr<Chunk>> chunks = level_loader_->GetGeneratedChunk();
    main_world_->world_interactions_.AddChunks(std::move(chunks));

    //Updated Light Info
    level_loader_->SendRequestedLightUpdates(std::move(main_world_->world_interactions_.GetRequestedLightUpdates()));
    std::vector<std::unique_ptr<LightStorage>> lighting = level_loader_->GetLightingInfomation();
    main_world_->world_interactions_.UpdateLighting(std::move(lighting));
}
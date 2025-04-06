#include "Level/Level.h"
#include "Level/Dimension/Dimensions/Overworld.h"

Level::Level() = default;
Level::~Level() = default;

void Level::Start(int worldGenThreadCount, int light_engine_thread_count_) {
    DimensionProperties properties;
    main_world_ = std::make_unique<OverworldDimension>(properties);
}

void Level::Stop() {
}

void Level::updateDimensions() {
    main_world_->Update();
}
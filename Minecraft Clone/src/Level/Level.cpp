#include "Level/Level.h"

#include <memory>

#include "Level/Dimension/Dimensions/Overworld.h"

Level::Level(GameContext& context) : context_{context} {}
Level::~Level() = default;

void Level::Start() {
  DimensionProperties properties;
  main_world_ = std::make_unique<OverworldDimension>(context_, properties);
}

void Level::Stop() {}

void Level::UpdateDimensions() { main_world_->Update(); }

void Level::ResetState() {}
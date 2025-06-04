#include "Level/Level.h"

#include <memory>

#include "Level/Dimension/Dimensions/Overworld.h"

Level::Level(GameContext& game_context) : game_context_{game_context} {}
Level::~Level() = default;

void Level::Start() {
  DimensionProperties properties;
  main_world_ = std::make_unique<OverworldDimension>(game_context_, properties);
}

void Level::Stop() {}

void Level::updateDimensions() { main_world_->Update(); }

#include "Level/Level.h"

#include <memory>

#include "Level/Dimension/Dimensions/Overworld.h"
#include "Level/ECS/ECSManager.h"
#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/EntitySystems.h"

Level::Level(GameContext& context) : context_{context} {}
Level::~Level() = default;

void Level::Start() {
  DimensionProperties properties;
  main_world_ = std::make_unique<OverworldDimension>(context_, properties);
  
  // Create test ECS entity
  main_world_->world_->GetECSManager().GetRegistry().CreateSand(glm::vec3(0, 70, 0));
  main_world_->world_->GetECSManager().GetSystems().CommitAll();
}

void Level::Stop() {}

void Level::UpdateDimensions() { main_world_->Update(); }

void Level::ResetState() {}
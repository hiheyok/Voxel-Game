#include "Level/Level.h"

#include <memory>

#include "Level/Dimension/DimensionProperties.h"
#include "Level/Dimension/Dimensions/Overworld.h"
#include "Level/ECS/EntityRegistry.h"
#include "Level/ECS/ServerECSManager.h"
#include "Level/ECS/ServerEntitySystems.h"

using std::make_unique;

Level::Level(GameContext& context) : context_{context} {}
Level::~Level() = default;

void Level::Start() {
  DimensionProperties properties;
  main_world_ = make_unique<OverworldDimension>(context_, properties);

  // Create test ECS entity (server-side only)
  auto& server_ecs =
      static_cast<ServerECSManager&>(main_world_->world_->GetECSManager());
  server_ecs.GetRegistry().CreateSand(glm::vec3(0, 70, 0));
  server_ecs.GetSystems().CommitAll();
  server_ecs.GetRegistry().Commit();
}

void Level::Stop() {}

void Level::UpdateDimensions() { main_world_->Update(); }

void Level::ResetState() {}
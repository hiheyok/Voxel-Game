#pragma once
#include <memory>
#include <vector>

#include "Core/Typenames.h"
#include "Level/TerrainGeneration/Generators/Generators.h"

class GameContext;
class WorldGenerator;

class GeneratorType {
 private:
  WorldGeneratorID RegisterWorldGenerator(std::unique_ptr<WorldGenerator> Gen);

  GameContext& context_;
  std::vector<std::unique_ptr<WorldGenerator>> generator_list_ = {};

 public:
  explicit GeneratorType(GameContext&);
  ~GeneratorType();

  WorldGeneratorID DEBUG =
      RegisterWorldGenerator(std::make_unique<DebugWorld>(context_));
  WorldGeneratorID DEBUG_MESH_STRESS =
      RegisterWorldGenerator(std::make_unique<DebugMeshStress>(context_));
  WorldGeneratorID DEBUG_MESH_STRESS_FACE =
      RegisterWorldGenerator(std::make_unique<DebugMeshStressFace>(context_));
  WorldGeneratorID DEBUG_2 =
      RegisterWorldGenerator(std::make_unique<DebugWorldSecond>(context_));
  WorldGeneratorID DEBUG_POSITIONING =
      RegisterWorldGenerator(std::make_unique<DebugPositioning>(context_));
  WorldGeneratorID MATH_WORLD =
      RegisterWorldGenerator(std::make_unique<MathWorld>(context_));
  WorldGeneratorID MOUNTAINS =
      RegisterWorldGenerator(std::make_unique<MountainGenerator>(context_));
  WorldGeneratorID SUPERFLAT =
      RegisterWorldGenerator(std::make_unique<SuperflatWorld>(context_));

  WorldGenerator* GetGenerator(WorldGeneratorID ID);
};

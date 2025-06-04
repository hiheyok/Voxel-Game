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

  GameContext& game_context_;
  std::vector<std::unique_ptr<WorldGenerator>> generator_list_ = {};

 public:
  explicit GeneratorType(GameContext&);
  ~GeneratorType();

  WorldGeneratorID DEBUG =
      RegisterWorldGenerator(std::make_unique<DebugWorld>(game_context_));
  WorldGeneratorID DEBUG_2 =
      RegisterWorldGenerator(std::make_unique<DebugWorldSecond>(game_context_));
  WorldGeneratorID DEBUG_POSITIONING =
      RegisterWorldGenerator(std::make_unique<DebugPositioning>(game_context_));
  WorldGeneratorID FAST_TERRAIN =
      RegisterWorldGenerator(std::make_unique<FastTerrain>(game_context_));
  WorldGeneratorID MATH_WORLD =
      RegisterWorldGenerator(std::make_unique<MathWorld>(game_context_));
  WorldGeneratorID MOUNTAINS = RegisterWorldGenerator(
      std::make_unique<MountainGenerator>(game_context_));
  WorldGeneratorID SUPERFLAT =
      RegisterWorldGenerator(std::make_unique<SuperflatWorld>(game_context_));

  WorldGenerator* GetGenerator(WorldGeneratorID ID);
};

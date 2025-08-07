#include "Level/TerrainGeneration/Generators/GeneratorType.h"

#include <memory>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Level/TerrainGeneration/Generators/Generators.h"
#include "Utils/LogUtils.h"

GeneratorType::GeneratorType(GameContext& context) : context_{context} {}

GeneratorType::~GeneratorType() = default;

WorldGeneratorID GeneratorType::RegisterWorldGenerator(
    std::unique_ptr<WorldGenerator> generator) {
  generator_list_.push_back(std::move(generator));
  context_.logger_->LogInfo(
      "GeneratorType::RegisterWorldGenerator",
      "Registered Generator: " + std::to_string(generator_list_.size() - 1));
  return generator_list_.size() - 1;
}

WorldGenerator* GeneratorType::GetGenerator(WorldGeneratorID ID) {
  return generator_list_[ID].get();
}

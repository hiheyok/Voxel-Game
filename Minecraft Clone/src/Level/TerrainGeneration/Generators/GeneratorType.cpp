#include "Level/TerrainGeneration/Generators/GeneratorType.h"

#include <cstddef>
#include <memory>
#include <utility>

#include "Core/GameContext/GameContext.h"
#include "Core/Typenames.h"
#include "Utils/LogUtils.h"

using std::move;
using std::unique_ptr;

GeneratorType::GeneratorType(GameContext& context) : context_{context} {}

GeneratorType::~GeneratorType() = default;

WorldGeneratorID GeneratorType::RegisterWorldGenerator(
    unique_ptr<WorldGenerator> generator) {
  generator_list_.push_back(move(generator));
  size_t id = generator_list_.size() - 1;
  LOG_INFO("Registered Generator: {}", id);
  return id;
}

WorldGenerator* GeneratorType::GetGenerator(WorldGeneratorID ID) {
  return generator_list_[ID].get();
}

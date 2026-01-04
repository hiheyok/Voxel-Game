#pragma once

#include <vector>
#include <string>

class GameContext;

namespace FileUtils {
  std::vector<char> ReadFileToBuffer(GameContext& context, const std::string& filename);
};
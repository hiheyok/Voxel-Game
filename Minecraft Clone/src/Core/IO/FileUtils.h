#pragma once

#include <vector>
#include <string>

class GameContext;

namespace FileUtils {
  std::vector<char> ReadFileToBuffer(GameContext& context, std::string filename);
};
#include "Core/IO/FileUtils.h"

#include <fstream>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

std::vector<char> FileUtils::ReadFileToBuffer(GameContext& context_,
                                              std::string filepath) {
  // Open the file at the end to easily get its size.
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  file.exceptions(std::fstream::badbit | std::fstream::failbit);

  if (!file.is_open()) {
    LOG_ERROR("Failed to open file: " + filepath);
    return {};  // Return empty vector on failure
  }

  std::streamsize size = file.tellg();
  if (size == 0) {
    LOG_WARN("File is empty: " + filepath);
    return {};
  }

  file.seekg(0, std::ios::beg);  // Go back to the beginning

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    LOG_ERROR("Failed to read file: " + filepath);
    return {};  // Return empty vector on failure
  }

  return buffer;
}

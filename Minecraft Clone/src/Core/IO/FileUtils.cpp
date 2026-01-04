#include "Core/IO/FileUtils.h"

#include <exception>
#include <fstream>
#include <ios>
#include <string>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

std::vector<char> FileUtils::ReadFileToBuffer(GameContext& context,
                                              std::string filepath) {
  try {
    // Open the file at the end to easily get its size.
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    file.exceptions(std::fstream::badbit | std::fstream::failbit);

    if (!file.is_open()) {
      LOG_STATIC_ERROR(context.logger_, "Failed to open file: {}", filepath);
      return {};  // Return empty vector on failure
    }

    std::streamsize size = file.tellg();
    if (size == 0) {
      LOG_STATIC_WARN(context.logger_, "File is empty: {}", filepath);
      return {};
    }

    file.seekg(0, std::ios::beg);  // Go back to the beginning

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
      LOG_STATIC_ERROR(context.logger_, "Failed to read file: {}", filepath);
      return {};  // Return empty vector on failure
    }

    return buffer;
  } catch (std::ios_base::failure& e) {
    LOG_STATIC_ERROR(context.logger_, "Failed to read file: {} | {}", filepath,
                     e.what());
  } catch (std::exception& e) {
    LOG_STATIC_ERROR(context.logger_, "Failed to read file: {} | {}", filepath,
                     e.what());
  }

  return {};
}

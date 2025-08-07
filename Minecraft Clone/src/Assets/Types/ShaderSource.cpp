#include "Assets/Types/ShaderSource.h"

#include "Core/IO/FileUtils.h"

ShaderSource::ShaderSource(GameContext& context, const std::string& asset_key,
                           std::string vertex, std::string fragment,
                           std::string geometry)
    : Asset{asset_key},
      context_{context},
      vert_path_{vertex},
      frag_path_{fragment},
      geo_path_{geometry} {}

ShaderSource::ShaderSource(GameContext& context, const std::string& asset_key,
                           std::string vertex, std::string fragment)
    : ShaderSource{context, asset_key, vertex, fragment, ""} {}

void ShaderSource::Load() {
  std::vector<char> vert_src =
      FileUtils::ReadFileToBuffer(context_, vert_path_);
  std::vector<char> frag_src =
      FileUtils::ReadFileToBuffer(context_, frag_path_);
  std::vector<char> geo_src;

  if (geo_path_.size() != 0) {
    geo_src = FileUtils::ReadFileToBuffer(context_, geo_path_);
  }

  // Add null terminating char
  vert_src.push_back('\0');
  frag_src.push_back('\0');
  geo_src.push_back('\0');

  vert_src_ = std::string(vert_src.data());
  frag_src_ = std::string(frag_src.data());

  if (geo_path_.size() != 0) {
    geo_src_ = std::string(frag_src.data());
  }
}

const std::string& ShaderSource::GetVertex() const noexcept {
  return vert_src_;
}

const std::string& ShaderSource::GetFragment() const noexcept {
  return frag_src_;
}

const std::string& ShaderSource::GetGeometry() const noexcept {
  return geo_src_;
}

#include "Assets/Types/ShaderSource.h"

#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Utils/Assert.h"
#include "Utils/LogUtils.h"

ShaderSource::ShaderSource(GameContext& context, const std::string& asset_key,
                           std::string vertex, std::string fragment,
                           std::string geometry)
    : Asset{asset_key},
      context_{context},
      type_{ShaderType::kShader},
      vert_path_{vertex},
      frag_path_{fragment},
      geo_path_{geometry} {}

ShaderSource::ShaderSource(GameContext& context, const std::string& asset_key,
                           std::string compute)
    : Asset{asset_key},
      context_{context},
      type_{ShaderType::kCompute},
      comp_path_{compute} {}

std::unique_ptr<ShaderSource> ShaderSource::CreateShader(
    GameContext& context, const std::string& asset_key, std::string vertex,
    std::string fragment, std::string geometry) {
  LOG_STATIC_DEBUG(context.logger_, "Loaded graphics shader: {}", asset_key);
  return std::make_unique<ShaderSource>(context, asset_key, vertex, fragment,
                                        geometry);
}

std::unique_ptr<ShaderSource> ShaderSource::CreateComputeShader(
    GameContext& context, const std::string& asset_key, std::string compute) {
  LOG_STATIC_DEBUG(context.logger_, "Loaded compute shader: {}", asset_key);
  return std::make_unique<ShaderSource>(context, asset_key, compute);
}

void ShaderSource::Load() {
  if (type_ == ShaderType::kShader) {
    LoadShader();
  } else {
    LoadCompute();
  }
}

const std::string& ShaderSource::GetVertex() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kShader, "Shader type must be kShader");
  return vert_src_;
}

const std::string& ShaderSource::GetFragment() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kShader, "Shader type must be kShader");
  return frag_src_;
}

const std::string& ShaderSource::GetGeometry() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kShader, "Shader type must be kShader");
  return geo_src_;
}

const std::string& ShaderSource::GetCompute() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kCompute, "Shader type must be kCompute");
  return comp_src_;
}

ShaderSource::ShaderType ShaderSource::GetType() const noexcept {
  return type_;
}

void ShaderSource::LoadCompute() {
  std::vector<char> comp_src =
      FileUtils::ReadFileToBuffer(context_, comp_path_);

  // Add null terminating char
  comp_src.push_back('\0');
  comp_src_ = std::string(comp_src.data());
}
void ShaderSource::LoadShader() {
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
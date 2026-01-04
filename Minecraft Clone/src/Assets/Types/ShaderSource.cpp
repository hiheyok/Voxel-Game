#include "Assets/Types/ShaderSource.h"

#include <memory>
#include <string>
#include <vector>

#include "Assets/Asset.h"
#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Utils/Assert.h"
#include "Utils/LogUtils.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

ShaderSource::ShaderSource(GameContext& context, const string& asset_key,
                           string vertex, string fragment, string geometry)
    : Asset{asset_key},
      context_{context},
      type_{ShaderType::kShader},
      vert_path_{vertex},
      frag_path_{fragment},
      geo_path_{geometry} {}

ShaderSource::ShaderSource(GameContext& context, const string& asset_key,
                           string compute)
    : Asset{asset_key},
      context_{context},
      type_{ShaderType::kCompute},
      comp_path_{compute} {}

unique_ptr<ShaderSource> ShaderSource::CreateShader(GameContext& context,
                                                    const string& asset_key,
                                                    string vertex,
                                                    string fragment,
                                                    string geometry) {
  LOG_STATIC_DEBUG(context.logger_, "Loaded graphics shader: {}", asset_key);
  return make_unique<ShaderSource>(context, asset_key, vertex, fragment,
                                   geometry);
}

unique_ptr<ShaderSource> ShaderSource::CreateComputeShader(
    GameContext& context, const string& asset_key, string compute) {
  LOG_STATIC_DEBUG(context.logger_, "Loaded compute shader: {}", asset_key);
  return make_unique<ShaderSource>(context, asset_key, compute);
}

void ShaderSource::Load() {
  if (type_ == ShaderType::kShader) {
    LoadShader();
  } else {
    LoadCompute();
  }
}

const string& ShaderSource::GetVertex() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kShader, "Shader type must be kShader");
  return vert_src_;
}

const string& ShaderSource::GetFragment() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kShader, "Shader type must be kShader");
  return frag_src_;
}

const string& ShaderSource::GetGeometry() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kShader, "Shader type must be kShader");
  return geo_src_;
}

const string& ShaderSource::GetCompute() const noexcept {
  GAME_ASSERT(type_ == ShaderType::kCompute, "Shader type must be kCompute");
  return comp_src_;
}

ShaderSource::ShaderType ShaderSource::GetType() const noexcept {
  return type_;
}

void ShaderSource::LoadCompute() {
  vector<char> comp_src = FileUtils::ReadFileToBuffer(context_, comp_path_);

  // Add null terminating char
  comp_src.push_back('\0');
  comp_src_ = string(comp_src.data());
}
void ShaderSource::LoadShader() {
  vector<char> vert_src = FileUtils::ReadFileToBuffer(context_, vert_path_);
  vector<char> frag_src = FileUtils::ReadFileToBuffer(context_, frag_path_);
  vector<char> geo_src;

  if (geo_path_.size() != 0) {
    geo_src = FileUtils::ReadFileToBuffer(context_, geo_path_);
  }

  // Add null terminating char
  vert_src.push_back('\0');
  frag_src.push_back('\0');
  geo_src.push_back('\0');

  vert_src_ = string(vert_src.data());
  frag_src_ = string(frag_src.data());

  if (geo_path_.size() != 0) {
    geo_src_ = string(frag_src.data());
  }
}
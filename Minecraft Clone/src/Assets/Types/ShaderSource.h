#pragma once

#include <memory>

#include "Assets/Asset.h"

class GameContext;

class ShaderSource : public Asset {
 public:
  enum class ShaderType { kShader, kCompute };

  explicit ShaderSource(GameContext& context, const std::string& asset_key,
                        std::string vertex, std::string fragment,
                        std::string geometry = "");

  explicit ShaderSource(GameContext& context, const std::string& asset_key,
                        std::string compute);

  static std::unique_ptr<ShaderSource> CreateShader(
      GameContext& context, const std::string& asset_key, std::string vertex,
      std::string fragment, std::string geometry = "");

  static std::unique_ptr<ShaderSource> CreateComputeShader(
      GameContext& context, const std::string& asset_key, std::string compute);

  ShaderSource();

  void Load() override;

  const std::string& GetVertex() const noexcept;
  const std::string& GetFragment() const noexcept;
  const std::string& GetGeometry() const noexcept;
  const std::string& GetCompute() const noexcept;

 private:
  void LoadCompute();
  void LoadShader();

  GameContext& context_;
  ShaderType type;

  std::string vert_path_;
  std::string frag_path_;
  std::string geo_path_;
  std::string comp_path_;

  std::string vert_src_;
  std::string frag_src_;
  std::string geo_src_;
  std::string comp_src_;
};

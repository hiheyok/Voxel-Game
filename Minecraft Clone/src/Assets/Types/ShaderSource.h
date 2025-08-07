#pragma once

#include "Assets/Asset.h"

class GameContext;

class ShaderSource : public Asset {
 public:
  explicit ShaderSource(GameContext& context, const std::string& asset_key,
                        std::string vertex, std::string fragment,
                        std::string geometry);

  explicit ShaderSource(GameContext& context, const std::string& asset_key,
                        std::string vertex, std::string fragment);

  ShaderSource();

  void Load() override;

  const std::string& GetVertex() const noexcept;
  const std::string& GetFragment() const noexcept;
  const std::string& GetGeometry() const noexcept;

 private:
  GameContext& context_;

  std::string vert_path_;
  std::string frag_path_;
  std::string geo_path_;

  std::string vert_src_;
  std::string frag_src_;
  std::string geo_src_;
};

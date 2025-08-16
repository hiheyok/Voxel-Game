#pragma once

#include <glm/vec4.hpp>
#include <string>

/*
The purpose of this class is to just hold the sprite data that is in the texture
atlas
*/

// be a render resource ?
class AtlasSprite {
 public:
  AtlasSprite(const std::string& sprite_name, glm::vec4 uv);

  glm::vec4 GetUV() const noexcept;
  const std::string& GetName() const noexcept;

 private:
  std::string sprite_name_;
  glm::vec4 uv_;
};

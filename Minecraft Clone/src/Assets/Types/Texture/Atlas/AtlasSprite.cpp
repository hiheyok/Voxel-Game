#include "Assets/Types/Texture/Atlas/AtlasSprite.h"

AtlasSprite::AtlasSprite(const std::string& sprite_name, glm::vec4 uv)
    : sprite_name_{sprite_name}, uv_{uv} {}

glm::vec4 AtlasSprite::GetUV() const noexcept {
  return uv_;
}

const std::string& AtlasSprite::GetName() const noexcept {
  return sprite_name_;
}

#include "Assets/Types/Texture/Atlas/AtlasSprite.h"

#include <glm/vec4.hpp>
#include <string>

using glm::vec4;
using std::string;

AtlasSprite::AtlasSprite(const string& sprite_name, vec4 uv)
    : sprite_name_{sprite_name}, uv_{uv} {}

vec4 AtlasSprite::GetUV() const noexcept { return uv_; }

const string& AtlasSprite::GetName() const noexcept { return sprite_name_; }

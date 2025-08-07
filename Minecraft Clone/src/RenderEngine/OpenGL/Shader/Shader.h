// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <string>

#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

class ShaderSource;

class Shader : public ShaderInterface {
 public:
  explicit Shader(
      GameContext&);  // TODO(hiheyok): tmp solution delete this later
  Shader(GameContext&, ShaderSource&);
};

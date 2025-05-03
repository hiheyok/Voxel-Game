// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <fstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>

#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

class Shader : public ShaderInterface {
 public:
  Shader();
  Shader(const char* vertexPath, const char* fragmentPath,
         const char* geometryPath = nullptr);
};

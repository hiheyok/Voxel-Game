// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>


#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"
#include "Utils/LogUtils.h"

class ComputeShader : public ShaderInterface {
 public:
  explicit ComputeShader(GameContext&, std::string source);

  void DispatchCompute(int x, int y, int z);
};

#pragma once
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <sstream>
#include <string>

#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"
#include "Utils/LogUtils.h"

class ComputeShader : public ShaderInterface {
   public:
    ComputeShader(std::string source);

    void DispatchCompute(int x, int y, int z);
};
#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>
#include <sstream>

#include "Utils/LogUtils.h"
#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

class ComputeShader : public ShaderInterface {
public:
    ComputeShader(std::string source);
    ~ComputeShader();

    void DispatchCompute(int x, int y, int z);
    void BindBufferAsSSBO(GLuint buffer, int idx);
    void SSBOMemoryBarrier();
};
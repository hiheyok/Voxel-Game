#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <fstream>

#include "Core/Typenames.h"
#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

class Shader : public ShaderInterface {
public:
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
};

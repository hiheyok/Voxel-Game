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

class ComputeShader {
public:
    ComputeShader(std::string source);

    ~ComputeShader();

    void Use();

    void Unuse();

    void BindBufferAsSSBO(GLuint buffer, int idx);

    void DispatchCompute(int x, int y, int z);

    void SSBOMemoryBarrier();

    void SetBool(const std::string& name, bool value);

    void SetInt(const std::string& name, int value);

    void SetFloat(const std::string& name, float value);

    void SetVec2(const std::string& name, const glm::vec2& value);

    void SetVec2(const std::string& name, float x, float y);

    void SetVec3(const std::string& name, const glm::vec3& value);

    void SetVec3(const std::string& name, float x, float y, float z);

    void SetIVec3(const std::string& name, const glm::ivec3& value);

    void SetIVec3(const std::string& name, int x, int y, int z);

    void SetVec4(const std::string& name, const glm::vec4& value);

    void SetVec4(const std::string& name, float x, float y, float z, float w);

    void SetMat2(const std::string& name, const glm::mat2& mat);

    void SetMat3(const std::string& name, const glm::mat3& mat);

    void SetMat4(const std::string& name, const glm::mat4& mat);

    void BindTexture2D(GLuint index, GLuint img, const std::string& name);

    void BindTextureArray2D(GLuint index, GLuint img, const std::string& name);
private:
    FastHashMap<std::string, int> cache_;

    GLint GetUniformLocation(std::string name);

    GLuint CreateComputeShader(const char* src);

    void CheckCompileError(GLuint shader);

    GLuint shader_id_ = 0;


};
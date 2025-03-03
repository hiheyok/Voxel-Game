#pragma once

#include <gl/glew.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include "../../../Utils/LogUtils.h"
#include "../../../Level/Typenames.h"

class Shader
{
public:
    unsigned int shader_id_ = NULL;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    void Init(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    // activate the shader
    // ------------------------------------------------------------------------
    void Use();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void SetBool(const std::string& name, bool value)
    {
        Use();
        glUniform1i(GetUniformLocation(name), (int)value);
    }
    // ------------------------------------------------------------------------
    void SetInt(const std::string& name, int value)
    {
        Use();
        glUniform1i(GetUniformLocation(name), value);
    }
    // ------------------------------------------------------------------------
    void SetFloat(const std::string& name, float value)
    {
        Use();
        glUniform1f(GetUniformLocation(name), value);
    }
    // ------------------------------------------------------------------------
    void SetVec2(const std::string& name, const glm::vec2& value)
    {
        Use();
        glUniform2fv(GetUniformLocation(name), 1, &value[0]);
    }
    void SetVec2(const std::string& name, float x, float y)
    {
        Use();
        glUniform2f(GetUniformLocation(name), x, y);
    }
    // ------------------------------------------------------------------------
    void SetVec3(const std::string& name, const glm::vec3& value)
    {
        Use();
        glUniform3fv(GetUniformLocation(name), 1, &value[0]);
    }
    void SetVec3(const std::string& name, float x, float y, float z)
    {
        Use();
        glUniform3f(GetUniformLocation(name), x, y, z);
    }

    void SetIVec3(const std::string& name, const glm::ivec3& value)
    {
        Use();
        glUniform3iv(GetUniformLocation(name), 1, &value[0]);
    }
    void SetIVec3(const std::string& name, int x, int y, int z)
    {
        Use();
        glUniform3i(GetUniformLocation(name), x, y, z);
    }
    // ------------------------------------------------------------------------
    void SetVec4(const std::string& name, const glm::vec4& value)
    {
        Use();
        glUniform4fv(GetUniformLocation(name), 1, &value[0]);
    }
    void SetVec4(const std::string& name, float x, float y, float z, float w)
    {
        Use();
        glUniform4f(GetUniformLocation(name), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void SetMat2(const std::string& name, const glm::mat2& mat)
    {
        Use();
        glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void SetMat3(const std::string& name, const glm::mat3& mat)
    {
        Use();
        glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void SetMat4(const std::string& name, const glm::mat4& mat)
    {
        Use();
        glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void BindTexture2D(GLuint index, GLuint img, const std::string& name) {

        SetInt(name, index);

        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, img);

    }

    void BindTextureArray2D(GLuint index, GLuint img, const std::string& name) {
        SetInt(name, index);

        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D_ARRAY, img);

    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------

    GLint GetUniformLocation(std::string name) {
        if (cache_.count(name)) {
            return cache_[name];
        }
        else {
            GLint location = glGetUniformLocation(shader_id_, name.c_str());
            cache_[name] = location;
            return location;
        }
    }

    FastHashMap<std::string, int> cache_;

    void CheckCompileErrors(GLuint shader, std::string type_)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type_ != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                g_logger.LogError("OpenGL Shader", "Failed to compile" + type_ + " Shader: \n" + std::string(infoLog) + "\n");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                g_logger.LogError("OpenGL Shader", "Failed to link Shader Program: \n" + std::string(infoLog) + "\n");
            }
        }
    }
};

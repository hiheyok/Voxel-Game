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
    unsigned int ShaderID = NULL;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    void init(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    // activate the shader
    // ------------------------------------------------------------------------
    void use();
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value)
    {
        use();
        glUniform1i(getUniformLocation(name), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value)
    {
        use();
        glUniform1i(getUniformLocation(name), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value)
    {
        use();
        glUniform1f(getUniformLocation(name), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value)
    {
        use();
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y)
    {
        use();
        glUniform2f(getUniformLocation(name), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value)
    {
        use();
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z)
    {
        use();
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    void setIVec3(const std::string& name, const glm::ivec3& value)
    {
        use();
        glUniform3iv(getUniformLocation(name), 1, &value[0]);
    }
    void setIVec3(const std::string& name, int x, int y, int z)
    {
        use();
        glUniform3i(getUniformLocation(name), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value)
    {
        use();
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        use();
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat)
    {
        use();
        glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat)
    {
        use();
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat)
    {
        use();
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    void bindTexture2D(GLuint index, GLuint img, const std::string& name) {

        setInt(name, index);

        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, img);

    }

    void bindTextureArray2D(GLuint index, GLuint img, const std::string& name) {
        setInt(name, index);

        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D_ARRAY, img);

    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------

    GLint getUniformLocation(std::string name) {
        if (cache.count(name)) {
            return cache[name];
        }
        else {
            GLint location = glGetUniformLocation(ShaderID, name.c_str());
            cache[name] = location;
            return location;
        }
    }

    FastHashMap<std::string, int> cache;

    void checkCompileErrors(GLuint shader, std::string type_)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type_ != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Logger.LogError("OpenGL Shader", "Failed to compile" + type_ + " Shader: \n" + std::string(infoLog) + "\n");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Logger.LogError("OpenGL Shader", "Failed to link Shader Program: \n" + std::string(infoLog) + "\n");
            }
        }
    }
};

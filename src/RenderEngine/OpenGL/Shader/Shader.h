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
#define _CRTDBG_MAP_ALLOC

class Shader
{
public:
    unsigned int ShaderID = NULL;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    void init(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            getLogger()->LogError("OpenGL Shader", "Failed to read file(s): " + std::string(vertexPath) + ", " + std::string(fragmentPath));
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "Vertex");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "Fragment");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "Geometry");
        }
        // shader Program
        ShaderID = glCreateProgram();
        glAttachShader(ShaderID, vertex);
        glAttachShader(ShaderID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ShaderID, geometry);
        glLinkProgram(ShaderID);
        checkCompileErrors(ShaderID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ShaderID);
        
    }
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

    std::unordered_map<std::string, int> cache;

    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                getLogger()->LogError("OpenGL Shader", "Failed to compile" + type + " Shader: \n" + std::string(infoLog) + "\n");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                getLogger()->LogError("OpenGL Shader", "Failed to link Shader Program: \n" + std::string(infoLog) + "\n");
            }
        }
    }
};

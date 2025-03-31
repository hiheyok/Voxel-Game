#include "Shader.h"
#include "../../../Utils/LogUtils.h"

void Shader::Init(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
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
        g_logger.LogError("Shader::Initr", "Failed to read file(s): " + std::string(vertexPath) + ", " + std::string(fragmentPath));
        g_logger.LogError("Shader::Init", e.what());
    }
    g_logger.LogDebug("Shader::Init", "Compiling shader: " + std::string(vertexPath) + ", " + std::string(fragmentPath));
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    CheckCompileErrors(vertex, "Vertex");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    CheckCompileErrors(fragment, "Fragment");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr)
    {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        CheckCompileErrors(geometry, "Geometry");
    }
    // shader Program
    shader_id_ = glCreateProgram();
    glAttachShader(shader_id_, vertex);
    glAttachShader(shader_id_, fragment);
    if (geometryPath != nullptr)
        glAttachShader(shader_id_, geometry);
    glLinkProgram(shader_id_);
    CheckCompileErrors(shader_id_, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}

void Shader::Use() {
    glUseProgram(shader_id_);
}

void Shader::SetBool(const std::string& name, bool value) {
    Use();
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, int value) {
    Use();
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) {
    Use();
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    Use();
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec2(const std::string& name, float x, float y) {
    Use();
    glUniform2f(GetUniformLocation(name), x, y);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    Use();
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec3(const std::string& name, float x, float y, float z) {
    Use();
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void Shader::SetIVec3(const std::string& name, const glm::ivec3& value) {
    Use();
    glUniform3iv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetIVec3(const std::string& name, int x, int y, int z) {
    Use();
    glUniform3i(GetUniformLocation(name), x, y, z);
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    Use();
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetVec4(const std::string& name, float x, float y, float z, float w) {
    Use();
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void Shader::SetMat2(const std::string& name, const glm::mat2& mat) {
    Use();
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const std::string& name, const glm::mat3& mat) {
    Use();
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) {
    Use();
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::BindTexture2D(GLuint index, GLuint img, const std::string& name) {
    SetInt(name, index);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, img);

}

void Shader::BindTextureArray2D(GLuint index, GLuint img, const std::string& name) {
    SetInt(name, index);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D_ARRAY, img);
}

GLint Shader::GetUniformLocation(std::string name) {
    if (cache_.count(name)) {
        return cache_[name];
    }
    else {
        GLint location = glGetUniformLocation(shader_id_, name.c_str());
        cache_[name] = location;
        return location;
    }
}


void Shader::CheckCompileErrors(GLuint shader, std::string type_) {
    GLint success;
    GLchar infoLog[1024];
    if (type_ != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            g_logger.LogError("Shader::CheckCompileErrors", "Failed to compile" + type_ + " Shader: \n" + std::string(infoLog) + "\n");
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            g_logger.LogError("Shader::CheckCompileErrors", "Failed to link Shader Program: \n" + std::string(infoLog) + "\n");
        }
    }
}
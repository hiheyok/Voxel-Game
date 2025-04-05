#include <fstream>

#include "RenderEngine/OpenGL/Shader/ShaderInterface.h"

ShaderInterface::ShaderInterface() = default;

ShaderInterface::~ShaderInterface() {
    if (shader_id_) {
        glDeleteProgram(shader_id_);
    }
}

void ShaderInterface::Use() {
    glUseProgram(shader_id_);
}

void ShaderInterface::SetBool(const std::string& name, bool value) {
    auto it = cache_bool_.find(name);
    if (it != cache_bool_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_bool_[name] = value;
    }
    Use();
    glUniform1i(GetUniformLocation(name), (int)value);
}

void ShaderInterface::SetInt(const std::string& name, int value) {
    auto it = cache_int_.find(name);
    if (it != cache_int_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_int_[name] = value;
    }
    Use();
    glUniform1i(GetUniformLocation(name), value);
}

void ShaderInterface::SetFloat(const std::string& name, float value) {
    auto it = cache_float_.find(name);
    if (it != cache_float_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_float_[name] = value;
    }
    Use();
    glUniform1f(GetUniformLocation(name), value);
}

void ShaderInterface::SetVec2(const std::string& name, const glm::vec2& value) {
    auto it = cache_vec2_.find(name);
    if (it != cache_vec2_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_vec2_[name] = value;
    }
    Use();
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void ShaderInterface::SetVec2(const std::string& name, float x, float y) {
    auto it = cache_vec2_.find(name);
    if (it != cache_vec2_.end()) {
        if (it->second == glm::vec2(x, y)) return;
        it->second = glm::vec2(x, y);
    }
    else {
        cache_vec2_[name] = glm::vec2(x, y);
    }
    Use();
    glUniform2f(GetUniformLocation(name), x, y);
}

void ShaderInterface::SetVec3(const std::string& name, const glm::vec3& value) {
    auto it = cache_vec3_.find(name);
    if (it != cache_vec3_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_vec3_[name] = value;
    }
    Use();
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void ShaderInterface::SetVec3(const std::string& name, float x, float y, float z) {
    auto it = cache_vec3_.find(name);
    if (it != cache_vec3_.end()) {
        if (it->second == glm::vec3(x, y, z)) return;
        it->second = glm::vec3(x, y, z);
    }
    else {
        cache_vec3_[name] = glm::vec3(x, y, z);
    }
    Use();
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void ShaderInterface::SetIVec3(const std::string& name, const glm::ivec3& value) {
    auto it = cache_ivec3_.find(name);
    if (it != cache_ivec3_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_ivec3_[name] = value;
    }
    Use();
    glUniform3iv(GetUniformLocation(name), 1, &value[0]);
}

void ShaderInterface::SetIVec3(const std::string& name, int x, int y, int z) {
    auto it = cache_ivec3_.find(name);
    if (it != cache_ivec3_.end()) {
        if (it->second == glm::ivec3(x, y, z)) return;
        it->second = glm::ivec3(x, y, z);
    }
    else {
        cache_ivec3_[name] = glm::ivec3(x, y, z);
    }
    Use();
    glUniform3i(GetUniformLocation(name), x, y, z);
}

void ShaderInterface::SetVec4(const std::string& name, const glm::vec4& value) {
    auto it = cache_vec4_.find(name);
    if (it != cache_vec4_.end()) {
        if (it->second == value) return;
        it->second = value;
    }
    else {
        cache_vec4_[name] = value;
    }
    Use();
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void ShaderInterface::SetVec4(const std::string& name, float x, float y, float z, float w) {
    auto it = cache_vec4_.find(name);
    if (it != cache_vec4_.end()) {
        if (it->second == glm::vec4(x, y, z, w)) return;
        it->second = glm::vec4(x, y, z, w);
    }
    else {
        cache_vec4_[name] = glm::vec4(x, y, z, w);
    }
    Use();
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void ShaderInterface::SetMat2(const std::string& name, const glm::mat2& mat) {
    auto it = cache_mat2_.find(name);
    if (it != cache_mat2_.end()) {
        if (it->second == mat) return;
        it->second = mat;
    }
    else {
        cache_mat2_[name] = mat;
    }
    Use();
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderInterface::SetMat3(const std::string& name, const glm::mat3& mat) {
    auto it = cache_mat3_.find(name);
    if (it != cache_mat3_.end()) {
        if (it->second == mat) return;
        it->second = mat;
    }
    else {
        cache_mat3_[name] = mat;
    }
    Use();
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderInterface::SetMat4(const std::string& name, const glm::mat4& mat) {
    auto it = cache_mat4_.find(name);
    if (it != cache_mat4_.end()) {
        if (it->second == mat) return;
        it->second = mat;
    }
    else {
        cache_mat4_[name] = mat;
    }
    Use();
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderInterface::BindTexture2D(GLuint index, GLuint img, const std::string& name) {
    SetInt(name, index);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, img);

}

void ShaderInterface::BindTextureArray2D(GLuint index, GLuint img, const std::string& name) {
    SetInt(name, index);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D_ARRAY, img);
}

GLint ShaderInterface::GetUniformLocation(std::string name) {
    if (cache_.count(name)) {
        return cache_[name];
    }
    else {
        GLint location = glGetUniformLocation(shader_id_, name.c_str());
        cache_[name] = location;
        return location;
    }
}

GLuint ShaderInterface::CompileShader(std::string source, std::string type, GLuint shaderType) {
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for errors
    CheckCompileErrors(shader, type);
    return shader;
}

void ShaderInterface::CheckCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, 0, infoLog);
            g_logger.LogError("Shader::CheckCompileErrors", "Failed to compile" + type + " Shader: \n" + std::string(infoLog) + "\n");
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, 0, infoLog);
            g_logger.LogError("Shader::CheckCompileErrors", "Failed to link Shader Program: \n" + std::string(infoLog) + "\n");
        }
    }
}

std::string ShaderInterface::ReadFile(std::string path) {
    std::ifstream file;
    std::string code;

    try {
        file.open(path);
        std::stringstream shaderStream;
        shaderStream << file.rdbuf();
        file.close();
        code = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        g_logger.LogError("Shader::ReadFile", "Failed to read file: " + std::string(path));
        g_logger.LogError("Shader::ReadFile", e.what());
    }

    return code;
}
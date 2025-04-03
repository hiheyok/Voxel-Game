#include "RenderEngine/OpenGL/Shader/ComputeShader.h"

ComputeShader::ComputeShader(std::string source) {
    std::string shaderSource = "";
    // First get the file
    try {
        std::ifstream file(source);
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream ss;
        ss << file.rdbuf();
        shaderSource = ss.str();
    }
    catch (const std::exception& e) {
        g_logger.LogError("ComputeShader::ComputeShader", "Failed to read file: " + std::string(source));
        g_logger.LogError("ComputeShader::ComputeShader", e.what());
    }

    shader_id_ = glCreateProgram();
    GLuint shader = CreateComputeShader(shaderSource.c_str());

    glAttachShader(shader_id_, shader);
    glLinkProgram(shader_id_);

    CheckCompileError(shader_id_);

    glDeleteShader(shader); // Cleanup
}

ComputeShader::~ComputeShader() {
    if (shader_id_) {
        glDeleteProgram(shader_id_);
    }
}

void ComputeShader::Use() {
    glUseProgram(shader_id_);
}

void ComputeShader::Unuse() {
    glUseProgram(0);
}

void ComputeShader::BindBufferAsSSBO(GLuint buffer, int idx) {
    Use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, idx, buffer);
}

void ComputeShader::DispatchCompute(int x, int y, int z) {
    Use();
    int maxWorkGroupCount[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxWorkGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &maxWorkGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &maxWorkGroupCount[2]);

    if (x > maxWorkGroupCount[0] || y > maxWorkGroupCount[1] || z > maxWorkGroupCount[2]) {
        g_logger.LogError("ComputeShader::DispatchCompute", "Work group size exceeds hardware limits.");
        return;
    }

    glDispatchCompute(x, y, z);
}

void ComputeShader::SSBOMemoryBarrier() {
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ComputeShader::SetBool(const std::string& name, bool value) {
    Use();
    glUniform1i(GetUniformLocation(name), (int)value);
}

void ComputeShader::SetInt(const std::string& name, int value) {
    Use();
    glUniform1i(GetUniformLocation(name), value);
}

void ComputeShader::SetFloat(const std::string& name, float value) {
    Use();
    glUniform1f(GetUniformLocation(name), value);
}

void ComputeShader::SetVec2(const std::string& name, const glm::vec2& value) {
    Use();
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}

void ComputeShader::SetVec2(const std::string& name, float x, float y) {
    Use();
    glUniform2f(GetUniformLocation(name), x, y);
}

void ComputeShader::SetVec3(const std::string& name, const glm::vec3& value) {
    Use();
    glUniform3fv(GetUniformLocation(name), 1, &value[0]);
}

void ComputeShader::SetVec3(const std::string& name, float x, float y, float z) {
    Use();
    glUniform3f(GetUniformLocation(name), x, y, z);
}

void ComputeShader::SetIVec3(const std::string& name, const glm::ivec3& value) {
    Use();
    glUniform3iv(GetUniformLocation(name), 1, &value[0]);
}

void ComputeShader::SetIVec3(const std::string& name, int x, int y, int z) {
    Use();
    glUniform3i(GetUniformLocation(name), x, y, z);
}

void ComputeShader::SetVec4(const std::string& name, const glm::vec4& value) {
    Use();
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void ComputeShader::SetVec4(const std::string& name, float x, float y, float z, float w) {
    Use();
    glUniform4f(GetUniformLocation(name), x, y, z, w);
}

void ComputeShader::SetMat2(const std::string& name, const glm::mat2& mat) {
    Use();
    glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::SetMat3(const std::string& name, const glm::mat3& mat) {
    Use();
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::SetMat4(const std::string& name, const glm::mat4& mat) {
    Use();
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::BindTexture2D(GLuint index, GLuint img, const std::string& name) {
    SetInt(name, index);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, img);

}

void ComputeShader::BindTextureArray2D(GLuint index, GLuint img, const std::string& name) {
    SetInt(name, index);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D_ARRAY, img);
}

GLint ComputeShader::GetUniformLocation(std::string name) {
    if (cache_.count(name)) {
        return cache_[name];
    }
    else {
        GLint location = glGetUniformLocation(shader_id_, name.c_str());
        cache_[name] = location;
        return location;
    }
}

GLuint ComputeShader::CreateComputeShader(const char* src) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        g_logger.LogError("ComputeShader::CreateComputeShader", "Compute Shader Compilation Failed:\n" + std::string(log));
    }

    return shader;
}

void ComputeShader::CheckCompileError(GLuint shader) {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, 0, infoLog);
        g_logger.LogError("ComputeShader::CheckCompileError", "Failed to link Shader Program: \n" + std::string(infoLog) + "\n");
    }
}
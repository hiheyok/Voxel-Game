#include "RenderEngine/OpenGL/Buffers/VertexArray.h"
#include "Utils/LogUtils.h"

void VertexArray::Delete() {
    glDeleteVertexArrays(1, &array_id_);
    array_id_ = NULL;
}

void VertexArray::Bind() {
    glBindVertexArray(array_id_);
}

void VertexArray::Unbind() {
    glBindVertexArray(0);
}

void VertexArray::EnableAttriPTR(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, int subIndex) {
    glVertexAttribPointer(index, size, type, normalized, sizeof(GL_FLOAT) * stride, (void*)(subIndex * sizeof(unsigned int)));
    glEnableVertexAttribArray(index);
}

void VertexArray::ResetArray() {
    glDeleteVertexArrays(1, &array_id_);
    glGenVertexArrays(1, &array_id_);
}

void VertexArray::GenArray() {
    glGenVertexArrays(1, &array_id_);
    g_logger.LogDebug("VertexArray::GenArray", "Created array. ID: " + std::to_string(array_id_));
}
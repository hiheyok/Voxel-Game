#pragma once

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&&) noexcept;
    VertexArray& operator=(VertexArray&&) noexcept;

    void Bind();
    void Unbind();
    void ResetArray();
    VertexArray& EnableAttriPTR(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, int subIndex);
private:
    unsigned int array_id_ = 0;
};
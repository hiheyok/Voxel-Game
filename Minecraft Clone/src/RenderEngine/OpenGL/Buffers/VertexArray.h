#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

class VertexArray {
public:
    void Delete();

    void GenArray();

    void Bind();

    void Unbind();

    void ResetArray();

    void EnableAttriPTR(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, int subIndex);
private:
    unsigned int array_id_ = 0;
};
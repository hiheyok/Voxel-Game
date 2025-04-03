#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

class Buffer {
public:
    void GenBuffer();

    void SetType(GLenum type);
    void SetMaxSize(size_t maxSize);
    void SetUsage(GLenum drawType);

    void ResetBuffer();

    void InitializeData();

    void Delete();

    void BindBase(int index);

    void UnbindBase(int index);

    void Bind();

    void Unbind();

    void getData(uint32_t* ptr, size_t offset, size_t size);

    void CopyFrom(Buffer buffer, size_t readOffset, size_t writeOffset, size_t size);

    unsigned int GetID();

    void InsertData(GLsizeiptr size, const void* data, GLenum usage);
    void InsertSubData(GLintptr Offset, GLsizeiptr Size, const void* Data);

    void SetStorage(int size);

    void CopyTo(Buffer& destination, size_t offset, size_t desOffset, size_t size);

    size_t max_size_ = 0;
    unsigned int buffer_id_ = 0;
    unsigned int type_ = 0, usage_ = 0;
};
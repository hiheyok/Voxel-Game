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

    size_t max_size_ = NULL;
    unsigned int buffer_id_ = NULL;
    unsigned int type_ = NULL, usage_ = NULL;
    
};

class BufferStorage {
public:

    BufferStorage();

    ~BufferStorage();

    void Delete();

    void Bind();
    void Unbind();


    void Create(GLuint bufferTarget, uint64_t size, bool dynamic = true, const void* data = nullptr);
    void InsertData(uint64_t offset, uint64_t size, const void* data);

    void CopyFrom(BufferStorage& sourceBuffer, size_t readOffset, size_t writeOffset, size_t size);

    void CopyTo(BufferStorage& destinationBuffer, size_t readOffset, size_t writeOffset, size_t size);

    bool IsInitialized() const;
    size_t GetMaxSize() const;
    
private:
    unsigned int buffer_storage_id_ = NULL;
    uint64_t max_size_ = NULL;
    GLenum target_ = NULL;
};

class VertexArray {
public:
    void Delete();

    void GenArray();

    void Bind();

    void Unbind();

    void ResetArray();

    void EnableAttriPTR(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, int subIndex);
private:
    unsigned int array_id_ = NULL;
};
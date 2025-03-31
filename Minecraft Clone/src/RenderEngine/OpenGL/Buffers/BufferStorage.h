#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>

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

    void CopyFrom(BufferStorage* sourceBuffer, size_t readOffset, size_t writeOffset, size_t size);
    void CopyTo(BufferStorage* destinationBuffer, size_t readOffset, size_t writeOffset, size_t size);

    bool IsInitialized() const;
    size_t GetMaxSize() const;

private:
    unsigned int buffer_storage_id_ = NULL;
    uint64_t max_size_ = NULL;
    GLenum target_ = NULL;
};
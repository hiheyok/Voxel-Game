#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <memory>

class ComputeShader;

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

    void MoveData(size_t readOffset, size_t writeOffset, size_t size);

    bool IsInitialized() const;
    size_t GetMaxSize() const;

private:
    static constexpr int kWorkGroupSize = 512;
    unsigned int buffer_storage_id_ = 0;
    uint64_t max_size_ = 0;
    GLenum target_ = 0;
    std::unique_ptr<ComputeShader> copy_shader_;
    std::unique_ptr<ComputeShader> move_shader_;
};
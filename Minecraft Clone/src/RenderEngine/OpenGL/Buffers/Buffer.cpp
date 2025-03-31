#include "Buffer.h"
#include "../../../Utils/LogUtils.h"

unsigned int Buffer::GetID() {
    return buffer_id_;
}
void Buffer::InsertData(GLsizeiptr size, const void* data, GLenum usage) {
    Bind();
    glBufferData(type_, size, data, usage);
    Unbind();
}

void Buffer::InsertSubData(GLintptr offset, GLsizeiptr size, const void* data) {
    Bind();
    glBufferSubData(type_, offset, size, data);
    Unbind();
}

void Buffer::ResetBuffer() {
    glDeleteBuffers(1, &buffer_id_);
    glGenBuffers(1, &buffer_id_);
}

void Buffer::Bind() {
    glBindBuffer(type_, buffer_id_);
}

void Buffer::Unbind() {
    glBindBuffer(type_, 0);
}

void Buffer::Delete() {
    glDeleteBuffers(1, &buffer_id_);
    buffer_id_ = NULL;
    max_size_ = NULL;
    type_ = NULL;
}

void Buffer::SetMaxSize(size_t maxSize) {
    max_size_ = maxSize;
}

void Buffer::SetType(GLenum type) {
    type_ = type;
}

void Buffer::SetUsage(GLenum usage) {
    usage_ = usage;
}

void Buffer::InitializeData() {
    Bind();
    glBufferData(type_, max_size_, nullptr, usage_);
    Unbind();
}

void Buffer::BindBase(int index) {
    glBindBufferBase(type_, index, buffer_id_);
}

void Buffer::UnbindBase(int index) {
    glBindBufferBase(type_, index, 0);
}

void Buffer::GenBuffer() {
    glGenBuffers(1, &buffer_id_);
    g_logger.LogDebug("Buffer::GenBuffer", "Created buffer. ID: " + std::to_string(buffer_id_));
}

void Buffer::getData(uint32_t* ptr, size_t offset, size_t size) {
    Bind();
    glGetBufferSubData(type_, offset, size, static_cast<void*>(ptr));
    Unbind();
}

void Buffer::CopyFrom(Buffer buffer, size_t readOffset, size_t writeOffset, size_t size) {
    glBindBuffer(GL_COPY_READ_BUFFER, buffer.buffer_id_);
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffer_id_);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void Buffer::CopyTo(Buffer& destination, size_t offset, size_t desOffset, size_t size) {
    Bind();
    destination.Bind();
//    glCopyNamedBufferSubData(BufferID, destination.BufferID, offset, desOffset, size);
    glCopyBufferSubData(type_, destination.type_, offset, desOffset, size);
    Unbind();
    destination.Unbind();
}

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

BufferStorage::BufferStorage() = default; // Add default constructor

BufferStorage::~BufferStorage() {
    if (buffer_storage_id_ != 0) {
        Delete();
    }
}

void BufferStorage::Delete() {
    if (buffer_storage_id_ != 0) {
        glDeleteBuffers(1, &buffer_storage_id_);
        buffer_storage_id_ = 0; // Use 0 to indicate deleted/uninitialized
        max_size_ = 0;
        target_ = 0;
        g_logger.LogDebug("BufferStorage::Delete", "Deleted buffer storage. ID was: " + std::to_string(buffer_storage_id_)); // Log before setting to 0
    }

}

void BufferStorage::Bind() {
    if (buffer_storage_id_ != 0) {
        glBindBuffer(target_, buffer_storage_id_);
    }
    else {
        g_logger.LogWarn("BufferStorage::Bind", "Attempted to bind an uninitialized/deleted buffer storage.");
    }
}

void BufferStorage::Unbind() {
    glBindBuffer(target_, 0);
}


void BufferStorage::Create(GLuint bufferTarget, uint64_t size, bool dynamic, const void* data) {
    if (buffer_storage_id_ != 0) {
        g_logger.LogWarn("BufferStorage::Create", "Create called on an already initialized buffer storage. Deleting old one. ID: " + std::to_string(buffer_storage_id_));
        Delete();
    }

    target_ = bufferTarget;
    max_size_ = size;

    glGenBuffers(1, &buffer_storage_id_);

    if (buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::Create", "glGenBuffers failed!");
        max_size_ = 0;
        target_ = 0;
        return;
    }

    g_logger.LogDebug("BufferStorage::Create", "Generated buffer storage. ID: " + std::to_string(buffer_storage_id_));
    Bind();

    // *** Use glBufferStorage ***
    GLbitfield flags = 0;
    if (dynamic) {
        flags |= GL_DYNAMIC_STORAGE_BIT; // Crucial for glBufferSubData/glCopyBufferSubData
    }
    // *** NO MAPPING FLAGS (GL_MAP_*) are included ***
    // This strongly hints the driver to keep the buffer in VRAM if possible.

    glBufferStorage(target_, static_cast<GLsizeiptr>(size), data, flags);

    // Check for OpenGL errors after buffer storage creation
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        g_logger.LogError("BufferStorage::Create", "OpenGL error after glBufferStorage: " + std::to_string(err));
        Delete(); // Clean up failed buffer
        return;
    }


    g_logger.LogInfo("BufferStorage::Create", "Created buffer storage ID: " + std::to_string(buffer_storage_id_)
        + " Size: " + std::to_string(size) + " bytes, Target: " + std::to_string(target_)
        + ", Dynamic: " + (dynamic ? "Yes" : "No"));

    /*glGenBuffers(1, &buffer_storage_id_);
    Bind();
    GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT;
    glBufferStorage(target_, size, nullptr, flags);*/
}

void BufferStorage::InsertData(uint64_t offset, uint64_t size, const void* data) {
    if (buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::InsertData", "Attempted InsertData on uninitialized buffer.");
        return;
    }

    if (offset + size > max_size_) {
        g_logger.LogError("BufferStorage::InsertData", "InsertData exceeds buffer bounds. Offset: " + std::to_string(offset)
            + ", Size: " + std::to_string(size) + ", MaxSize: " + std::to_string(max_size_));
        return;
    }

    Bind();
    glBufferSubData(target_, offset, size, data);
}

void BufferStorage::CopyFrom(BufferStorage& sourceBuffer, size_t readOffset, size_t writeOffset, size_t size) {
    if (buffer_storage_id_ == 0 || sourceBuffer.buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::CopyFrom", "Attempted CopyFrom with uninitialized buffer(s).");
        return;
    }
    if (writeOffset + size > max_size_ || readOffset + size > sourceBuffer.max_size_) {
        g_logger.LogError("BufferStorage::CopyFrom", "CopyFrom exceeds buffer bounds.");
        return;
    }
    glBindBuffer(GL_COPY_READ_BUFFER, sourceBuffer.buffer_storage_id_);
    glBindBuffer(GL_COPY_WRITE_BUFFER, buffer_storage_id_);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void BufferStorage::CopyTo(BufferStorage& destinationBuffer, size_t readOffset, size_t writeOffset, size_t size) {
    if (buffer_storage_id_ == 0 || destinationBuffer.buffer_storage_id_ == 0) {
        g_logger.LogError("BufferStorage::CopyTo", "Attempted CopyTo with uninitialized buffer(s).");
        return;
    }
    if (readOffset + size > max_size_ || writeOffset + size > destinationBuffer.max_size_) {
        g_logger.LogError("BufferStorage::CopyTo", "CopyTo exceeds buffer bounds.");
        return;
    }
    glBindBuffer(GL_COPY_READ_BUFFER, buffer_storage_id_);
    glBindBuffer(GL_COPY_WRITE_BUFFER, destinationBuffer.buffer_storage_id_);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, readOffset, writeOffset, size);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

bool BufferStorage::IsInitialized() const { return buffer_storage_id_ != 0; }

size_t BufferStorage::GetMaxSize() const { return max_size_; }
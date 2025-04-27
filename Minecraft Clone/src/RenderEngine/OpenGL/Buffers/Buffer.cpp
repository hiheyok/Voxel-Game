#include "RenderEngine/OpenGL/Buffers/Buffer.h"
#include "Utils/LogUtils.h"

Buffer::Buffer() {
    glGenBuffers(1, &buffer_id_);
    g_logger.LogDebug("Buffer::GenBuffer", "Created buffer. ID: " + std::to_string(buffer_id_));
}

Buffer::~Buffer() {
    if (buffer_id_ != 0) {
        glDeleteBuffers(1, &buffer_id_);
    }
}

Buffer::Buffer(Buffer&& buffer) noexcept {
    (*this) = std::move(buffer);
}

Buffer& Buffer::operator=(Buffer&& buffer) noexcept {
    max_size_ = buffer.max_size_;
    buffer_id_ = buffer.buffer_id_;
    type_ = buffer.type_;
    usage_ = buffer.usage_;

    buffer.max_size_ = 0;
    buffer.buffer_id_ = 0;
    buffer.type_ = 0;
    buffer.usage_ = 0;
    return *this;
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

void Buffer::GetData(uint32_t* ptr, size_t offset, size_t size) {
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

Buffer::operator unsigned int() const noexcept {
    return buffer_id_;
}

unsigned int Buffer::GetId() const noexcept {
    return buffer_id_;
}
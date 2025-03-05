#include "Buffer.h"

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
    g_logger.LogDebug("Buffer", "Created buffer. ID: " + std::to_string(buffer_id_));
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
    g_logger.LogDebug("Vertex Array", "Created array. ID: " + std::to_string(array_id_));
}
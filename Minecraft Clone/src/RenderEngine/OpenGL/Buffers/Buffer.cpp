#include "Buffer.h"

unsigned int Buffer::GetID() {
	return BufferID;
}
void Buffer::InsertData(GLsizeiptr Size, const void* Data, GLenum Usage) {
	Bind();
	glBufferData(Type, Size, Data, Usage);
	Unbind();
}

void Buffer::InsertSubData(GLintptr Offset, GLsizeiptr Size, const void* Data) {
	Bind();
	glBufferSubData(Type, Offset, Size, Data);
	Unbind();
}

void Buffer::ResetBuffer() {
	glDeleteBuffers(1, &BufferID);
	glGenBuffers(1, &BufferID);
}

void Buffer::Bind() {
	glBindBuffer(Type, BufferID);
}

void Buffer::Unbind() {
	glBindBuffer(Type, 0);
}

void Buffer::Delete() {
	glDeleteBuffers(1, &BufferID);
	BufferID = NULL;
	MaxSize = NULL;
	Type = NULL;
}

void Buffer::SetMaxSize(size_t MaxSize_) {
	MaxSize = MaxSize_;
}

void Buffer::SetType(GLenum Type_) {
	Type = Type_;
}

void Buffer::SetUsage(GLenum Usage_) {
	Usage = Usage_;
}

void Buffer::InitializeData() {
	Bind();
	glBufferData(Type, MaxSize, nullptr, Usage);
	Unbind();
}

void Buffer::BindBase(int Index) {
	glBindBufferBase(Type, Index, BufferID);
}

void Buffer::UnbindBase(int Index) {
	glBindBufferBase(Type, Index, 0);
}

void Buffer::GenBuffer() {
	glGenBuffers(1, &BufferID);
	Logger.LogDebug("Buffer", "Created buffer. ID: " + std::to_string(BufferID));
}

void Buffer::getData(uint32_t* ptr, size_t offset, size_t size) {
	Bind();
	glGetBufferSubData(Type, offset, size, static_cast<void*>(ptr));
	Unbind();
}

void Buffer::CopyTo(Buffer& destination, size_t offset, size_t desOffset, size_t size) {
	Bind();
	destination.Bind();
//	glCopyNamedBufferSubData(BufferID, destination.BufferID, offset, desOffset, size);
	glCopyBufferSubData(Type, destination.Type, offset, desOffset, size);
	Unbind();
	destination.Unbind();
}

void VertexArray::Delete() {
	glDeleteVertexArrays(1, &ArrayID);
	ArrayID = NULL;
}

void VertexArray::Bind() {
	glBindVertexArray(ArrayID);
}

void VertexArray::Unbind() {
	glBindVertexArray(0);
}

void VertexArray::EnableAttriPTR(GLuint Index, GLint Size, GLenum Type, GLboolean normalized, GLsizei Stride, int SubIndex) {
	glVertexAttribPointer(Index, Size, Type, normalized, sizeof(GL_FLOAT) * Stride, (void*)(SubIndex * sizeof(unsigned int)));
	glEnableVertexAttribArray(Index);
}

void VertexArray::ResetArray() {
	glDeleteVertexArrays(1, &ArrayID);
	glGenVertexArrays(1, &ArrayID);
}

void VertexArray::GenArray() {
	glGenVertexArrays(1, &ArrayID);
	Logger.LogDebug("Vertex Array", "Created array. ID: " + std::to_string(ArrayID));
}
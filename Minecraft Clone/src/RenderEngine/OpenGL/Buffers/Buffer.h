#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../../../Utils/LogUtils.h"



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
	GLenum type_ = NULL, usage_ = NULL;
	
};

class BufferStorage {
public:

	void Delete() {
		glDeleteBuffers(1, &buffer_storage_id_);
	}

	void Bind() {
		glBindBuffer(target_, buffer_storage_id_);
	}

	void Unbind() {
		glBindBuffer(target_, 0);
	}

	void Create(GLuint bufferTarget, uint64_t size) {
		target_ = bufferTarget;
		max_size_ = size;

		glGenBuffers(1, &buffer_storage_id_);
		Bind();
		GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_DYNAMIC_STORAGE_BIT;
		glBufferStorage(target_, size, nullptr, flags);
	}

	void InsertData(uint64_t Offset, uint64_t Size, const void* data) {
		Bind();
		glBufferSubData(target_, Offset, Size, data);
		Unbind();
	}

	void CopyFrom(Buffer& buffer, size_t readOffset, size_t writeOffset, size_t size) {
		Bind();
		buffer.Bind();
		glCopyBufferSubData(buffer.type_, target_, readOffset, writeOffset, size);
		Unbind();
		buffer.Unbind();
	}

	void CopyTo(Buffer& buffer, size_t ReadOffset, size_t WriteOffset, size_t Size) {
		Bind();
		buffer.Bind();
		glCopyBufferSubData(target_, buffer.type_, ReadOffset, WriteOffset, Size);
		Unbind();
		buffer.Unbind();
	}
	
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
#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../../../Utils/LogUtils.h"



class Buffer {
public:
	void GenBuffer();

	void SetType(GLenum Type_);
	void SetMaxSize(size_t MaxSize_);
	void SetUsage(GLenum DrawType_);

	void ResetBuffer();

	void InitializeData();

	void Delete();

	void BindBase(int Index);

	void UnbindBase(int Index);

	void Bind();

	void Unbind();

	void getData(uint32_t* ptr, size_t offset, size_t size);

	void CopyFrom(Buffer buffer, size_t ReadOffset, size_t WriteOffset, size_t Size);

	unsigned int GetID();

	void InsertData(GLsizeiptr Size, const void* Data, GLenum Usage);
	void InsertSubData(GLintptr Offset, GLsizeiptr Size, const void* Data);

	void SetStorage(int size);

	void CopyTo(Buffer& destination, size_t offset, size_t desOffset, size_t size);

	size_t MaxSize = NULL;
	unsigned int BufferID = NULL;
	GLenum Type = NULL, Usage = NULL;
	
};

class BufferStorage {
public:

	void Delete() {
		glDeleteBuffers(1, &BufferStorageID);
	}

	void Bind() {
		glBindBuffer(Target, BufferStorageID);
	}

	void Unbind() {
		glBindBuffer(Target, 0);
	}

	void Create(GLuint BufferTarget, uint64_t Size) {
		Target = BufferTarget;
		MaxSize = Size;

		glGenBuffers(1, &BufferStorageID);
		Bind();
		GLbitfield flags = GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT| GL_DYNAMIC_STORAGE_BIT;
		glBufferStorage(Target, Size, nullptr, flags);
	}

	void InsertData(uint64_t Offset, uint64_t Size, const void* data) {
		Bind();
		glBufferSubData(Target, Offset, Size, data);
		Unbind();
	}

	void CopyFrom(Buffer& buffer, size_t ReadOffset, size_t WriteOffset, size_t Size) {
		Bind();
		buffer.Bind();
		glCopyBufferSubData(buffer.Type, Target, ReadOffset, WriteOffset, Size);
		Unbind();
		buffer.Unbind();
	}

	void CopyTo(Buffer& buffer, size_t ReadOffset, size_t WriteOffset, size_t Size) {
		Bind();
		buffer.Bind();
		glCopyBufferSubData(Target, buffer.Type, ReadOffset, WriteOffset, Size);
		Unbind();
		buffer.Unbind();
	}
	
private:
	unsigned int BufferStorageID = NULL;
	uint64_t MaxSize = NULL;
	GLenum Target = NULL;
};

class VertexArray {
public:
	void Delete();

	void GenArray();

	void Bind();

	void Unbind();

	void ResetArray();

	void EnableAttriPTR(GLuint Index, GLint Size, GLenum Type, GLboolean normalized, GLsizei Stride, int SubIndex);
private:
	unsigned int ArrayID = NULL;
};
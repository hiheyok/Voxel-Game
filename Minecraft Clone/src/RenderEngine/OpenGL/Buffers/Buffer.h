#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Buffer {
public:
	Buffer() {
		glGenBuffers(1, &BufferID);
	}
	~Buffer() {
		glDeleteBuffers(1, &BufferID);
	}

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

	unsigned int GetID();

	void InsertData(GLsizeiptr Size, const void* Data, GLenum Usage);
	void InsertSubData(GLintptr Offset, GLsizeiptr Size, const void* Data);
private:
	unsigned int BufferID = NULL;
	GLenum Type = NULL, Usage = NULL;
	size_t MaxSize = NULL;
};

class VertexArray {
public:
	VertexArray() {
		glGenVertexArrays(1, &ArrayID);
	}
	~VertexArray() {
		glDeleteVertexArrays(1, &ArrayID);
	}

	void GenArray();

	void Bind();

	void Unbind();

	void ResetArray();

	void EnableAttriPTR(GLuint Index, GLint Size, GLenum Type, GLboolean normalized, GLsizei Stride, int SubIndex);
private:
	unsigned int ArrayID = NULL;
};
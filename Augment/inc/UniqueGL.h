#pragma once

#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

template<void(*func)(GLuint)>
struct GLDeleter{
	using pointer = GLuint;
	void operator()(GLuint p) const {
		func(p);
	}
};

void DeleteBuffer(GLuint p);
void DeleteProgram(GLuint p);
void DeleteShader(GLuint p);
void DeleteTexture(GLuint p);
void DeleteVertexArray(GLuint p);

using UniqueBuffer = std::unique_ptr<GLuint, GLDeleter<DeleteBuffer>>;
using UniqueProgram = std::unique_ptr<GLuint, GLDeleter<DeleteProgram>>;
using UniqueShader = std::unique_ptr<GLuint, GLDeleter<DeleteShader>>;
using UniqueTexture = std::unique_ptr<GLuint, GLDeleter<DeleteTexture>>;
using UniqueVertexArray = std::unique_ptr<GLuint, GLDeleter<DeleteVertexArray>>;

GLuint glGenBuffer();
GLuint glGenTexture();
GLuint glGenVertexArray();


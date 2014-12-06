#pragma once
#include <GL/glew.h>

#include <stdio.h>
#include <vector>

#include "Utils.h"
#include "UniqueGL.h"


class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();
	void LoadShader(const std::string& filename, GLenum type);
	void Link();
	void Bind();
	void Unbind();

private:

	std::vector<UniqueShader> shaders;
	UniqueProgram program;

	void ShaderCompileTest(GLuint shader);
};


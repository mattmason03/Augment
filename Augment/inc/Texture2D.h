#pragma once

#include <string>
#include <memory>

#include <GL/glew.h>

#include "UniqueGL.h"

class Texture2D
{
public:

	//Initialize a texture from a file
	Texture2D(const std::string &filename);
	//Texture2D();
	~Texture2D();

	int GetHeight();
	int GetWidth();
	GLuint GetHandle();

	Texture2D(const Texture2D&) = delete;
	Texture2D& operator=(const Texture2D) = delete;

private:

	int height, width;
	GLenum sizeFormat, format;

	UniqueTexture texture;

};
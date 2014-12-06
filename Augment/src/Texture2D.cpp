#include "Texture2D.h"

#include <STB\stb_image.h>

Texture2D::Texture2D(const std::string& filename)
	:texture(glGenTexture())
{
	int numComponents;
	
	struct STBIDeleter{
		void operator()(unsigned char* p){
			stbi_image_free(p);
		}
	};
	//get the data from texture 'filename'
	std::unique_ptr<unsigned char, STBIDeleter> textureData(stbi_load(filename.c_str(), &width, &height, &numComponents, 0));

	switch (numComponents)
	{
	case 1:
		sizeFormat = GL_R8;
		format = GL_R;
		break;

	case 2:
		sizeFormat = GL_RG8;
		format = GL_RG;
		break;

	case 3:
		sizeFormat = GL_RGB8;
		format = GL_RGB;
		break;

	default:
		sizeFormat = GL_RGBA8;
		format = GL_RGBA;
		break;
	}
	
	glBindTexture(GL_TEXTURE_2D, texture.get());
	glTexStorage2D(GL_TEXTURE_2D, 1, sizeFormat, width, height);

	//load texture with data
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, textureData.get());

	//set linear filtering for scaling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture2D::~Texture2D()
{
}

unsigned int Texture2D::GetHandle(){
	return texture.get();
}

int Texture2D::GetHeight(){
	return height;
}

int Texture2D::GetWidth(){
	return width;
}
#include "UniqueGL.h"

void DeleteBuffer(GLuint p){
	glDeleteBuffers(1, &p);
}

void DeleteProgram(GLuint p){
	glDeleteProgram(p);
}

void DeleteShader(GLuint p){
	glDeleteShader(p);
}

void DeleteTexture(GLuint p){
	glDeleteTextures(1, &p);
}

void DeleteVertexArray(GLuint p){
	glDeleteVertexArrays(1, &p);
}

GLuint glGenBuffer(){
	GLuint ret;
	glGenBuffers(1, &ret);
	return ret;
}

GLuint glGenTexture(){
	GLuint ret;
	glGenTextures(1, &ret);
	return ret;
}

GLuint glGenVertexArray(){
	GLuint ret;
	glGenVertexArrays(1, &ret);
	return ret;
}

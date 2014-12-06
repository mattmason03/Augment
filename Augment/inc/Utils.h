#pragma once

#include <fstream>
#include <string>

#include <GL/glew.h>

static std::string LoadFromFile(const std::string fileName){

	std::ifstream file;
	file.open((fileName).c_str());

	std::string line;
	std::string fileContent;

	if (file.is_open()){
		while (file.good()){
			getline(file, line);
			fileContent.append(line + '\n');
		}
	}
	file.close();
	return fileContent;
};

static void PrintGLErrors(){
	GLenum err = glGetError();
	while (err != GL_NO_ERROR){
		printf("%i\n", err);
		err = glGetError();
	}
};
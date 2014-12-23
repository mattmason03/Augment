#pragma once

#include <fstream>
#include <string>
#include <iostream>

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

#ifdef _DEBUG
#define LOG(x,y) LogError(x,y)
#define LOGGL(x) LogErrorGL(x)
#else
#define LOG(x,y)
#define LOGGL(x)
#endif

static void LogError(std::string prefix, std::string message){
	std::cout << prefix << ": " << message << std::endl;
}

static void LogErrorGL(std::string prefix){
	GLenum err = glGetError();
	while (err != GL_NO_ERROR){
		std::cout << prefix << ": " << err << std::endl;
		err = glGetError();
	}
};
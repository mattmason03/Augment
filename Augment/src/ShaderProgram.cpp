#include "ShaderProgram.h"


ShaderProgram::ShaderProgram()
	:program(glCreateProgram())
{
}


ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::LoadShader(const std::string& filename, GLenum type){
	UniqueShader shader(glCreateShader(type));
	GLuint s = shader.get();

	std::string shaderText = LoadFromFile(filename);
	const char *c_str = shaderText.c_str();
	glShaderSource(s, 1, &c_str, NULL);
	glCompileShader(s);
	

	//Test if shader compiled
	GLint maxLength = 0;
	glGetShaderiv(s, GL_INFO_LOG_LENGTH, &maxLength);

	//The maxLength includes the NULL character
	std::vector<char> errorLog(maxLength);
	glGetShaderInfoLog(s, maxLength, &maxLength, &errorLog[0]);

	std::string str(errorLog.begin(), errorLog.end());
	printf("%s\n", str.c_str());

	glAttachShader(program.get(), s);
	shaders.emplace_back(std::move(shader));
}

void ShaderProgram::Link(){
	glLinkProgram(program.get());
	shaders.clear();
}

void ShaderProgram::Bind(){
	glUseProgram(program.get());
}

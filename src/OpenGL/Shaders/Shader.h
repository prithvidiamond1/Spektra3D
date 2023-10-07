#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>

class Shader {
private:
	unsigned int shaderProgramID;

	void checkShaderCompilation(unsigned int ID, std::string type);

public:
	Shader();
	
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

	unsigned int getID();

	void use();
};
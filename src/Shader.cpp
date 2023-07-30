#include "Shader.h"

void Shader::checkShaderCompilation(unsigned int ID, std::string type)
{
	int success;
	char infoLog[1024];

	if (type.compare("PROGRAM") != 0) {
		glGetShaderiv(this->shaderProgramID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(this->shaderProgramID, 1024, NULL, infoLog);
			std::cout << "ERROR - " << type << " shader compilation failed: \n\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(this->shaderProgramID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(this->shaderProgramID, 1024, NULL, infoLog);
			std::cout << "ERROR - shader " << type << " linking failed: \n\n" << infoLog << std::endl;
		}
	}
}

Shader::Shader()
{
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexShaderFile.open(vertexShaderPath);
		fragmentShaderFile.open(fragmentShaderPath);

		std::stringstream vertexShaderStream, fragmentShaderStream;

		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexShaderCode = vertexShaderStream.str();
		fragmentShaderCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure& ex) {
		std::cout << "ERROR - shader files could not be read: \n\n" << ex.what() << std::endl;
	}

	const char* rawVertexShaderCode = vertexShaderCode.c_str();
	const char* rawFragmentShaderCode = fragmentShaderCode.c_str();

	unsigned int vertexShaderID, fragmentShaderID;
	
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &rawVertexShaderCode, NULL);
	glCompileShader(vertexShaderID);
	checkShaderCompilation(vertexShaderID, "VERTEX");

	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &rawFragmentShaderCode, NULL);
	glCompileShader(fragmentShaderID);
	checkShaderCompilation(fragmentShaderID, "FRAGMENT");

	this->shaderProgramID = glCreateProgram();
	glAttachShader(this->shaderProgramID, vertexShaderID);
	glAttachShader(this->shaderProgramID, fragmentShaderID);
	glLinkProgram(this->shaderProgramID);
	checkShaderCompilation(this->shaderProgramID, "PROGRAM");

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

unsigned int Shader::getID()
{
	return this->shaderProgramID;
}

void Shader::use()
{
	glUseProgram(this->shaderProgramID);
}

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// ID of the shader
	unsigned int ID;

	// Constructor
	Shader(const char* vertexPath, const char* fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vertShaderFile;
		std::ifstream fragShaderFile;

		// ensure that the ifstream objects can throw exceptions
		vertShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vertShaderFile.open(vertexPath);
			fragShaderFile.open(fragmentPath);

			// read file's contents into streams
			std::stringstream vSStream, fSStream;
			vSStream << vertShaderFile.rdbuf();
			fSStream << fragShaderFile.rdbuf();

			// close files
			vertShaderFile.close();
			fragShaderFile.close();

			// convert sstreams into strings
			vertexCode = vSStream.str();
			fragmentCode = fSStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "SHADER FILE READ ERRROR!!!" << std::endl;
		}

		// SHADER BUILDING AND LINKING PIPELINE
		const char* vertexCode_C = vertexCode.c_str();
		const char* fragmentCode_C = fragmentCode.c_str();

		// 1. Compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexCode_C, NULL);
		glCompileShader(vertex);
		checkForErrors(vertex, "VERTEX");
		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragmentCode_C, NULL);
		glCompileShader(fragment);
		checkForErrors(fragment, "FRAGMENT");

		// 2. Link shaders and create program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		checkForErrors(ID, "PROGRAM");

		// 3. Delete individual shaders
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		// ------------------------------------
	}
	void use()
	{
		glUseProgram(ID);
	}
	void setInt(const std::string& name, int value) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform1i(location, value);
	}
	void setFloat(const std::string& name, float value) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform1f(location, value);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform3f(location, x, y, z);
	}
	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniform3fv(location, 1, &value[0]);
	}
	void setMat4(const std::string& name, const glm::mat4& value) const
	{
		int location = glGetUniformLocation(ID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
	}
private:
	void checkForErrors(unsigned int shaderCode, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shaderCode, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shaderCode, 1024, NULL, infoLog);
				std::cout << "SHADER COMPILATION ERROR --- " << type << std::endl << infoLog
					<< std::endl << "-- --------------------------- --" << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shaderCode, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shaderCode, 1024, NULL, infoLog);
				std::cout << "SHADER LINKING ERROR --- " << type << std::endl << infoLog
					<< std::endl << "-- --------------------------- --" << std::endl;
			}
		}
	}
};

#endif
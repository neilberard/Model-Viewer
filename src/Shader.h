#pragma once

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <map>
#include "Debugging.h"
#include "glm/glm.hpp"




struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;

};


class Shader
{
private:
	unsigned int mRendererID = 0;
	std::string mFilePath;
	std::unordered_map<std::string, GLint> mUniformLocationCache;
	bool mValid = false;
	bool mBinded = false;

	unsigned int mUniformBlock = 0;
	unsigned int mFragmentBegin = 0;  // Store the line number of the vertex shader for debugging.
	unsigned int mVertexBegin = 0; // Store the line number of the fragment shader for debugging.

public:
	Shader() {};
	Shader(const std::string& filepath);
	Shader(const std::string& filepath, unsigned int pUniformBlock, const std::string& pUniformBlockStr);
	~Shader();
	
	std::string getFilePath() { return mFilePath; }

	void bindShader();
	void unbindShader();
	void Reload();


	void SetUniform1i(const std::string& name, int value);
	void SetUniform1i(const std::string& name, unsigned int value);
	void SetUniform1i(const std::string& name, bool value);
	template <class T> void SetUniform1i(const std::string& name, T value);

	void SetUniform1f(const std::string& name, float value);
	template <class T> void SetUniform1f(const std::string& name, T value);



	void SetUniform3f(const std::string& name, glm::vec3 value);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, glm::vec4 value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);


private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	GLint GetUniformLocation(const std::string& name);
};


// Catch incorrect data types from being implicitly converted. IE int -> float.
template <class T>
void Shader::SetUniform1i(const std::string& name, T value)
{
	LOG_ERROR("Wrong Data type, expecting integer, unsigned integer or bool. Got: {}", typeid(value).name());
}

template <class T>
void Shader::SetUniform1f(const std::string& name, T value)
{
	LOG_ERROR("Wrong Data type, expecting float. Got: {}", typeid(value).name());
}

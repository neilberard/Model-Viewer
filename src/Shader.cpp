#include "Shader.h"
#include "Log.h"
#include <filesystem>
#include <iostream>

Shader::Shader(const std::string& filepath)
	: mFilePath(filepath), mRendererID(0), mBinded(false)
{
	std::string p = std::filesystem::absolute(filepath).string();
	mFilePath = p;

	ShaderProgramSource source = Shader::ParseShader(mFilePath);
	mRendererID = CreateShader(source.VertexSource, source.FragmentSource);	
	mValid = true;
}


Shader::Shader(const std::string& filepath, unsigned int pUniformBlock, const std::string& pUniformBlockStr)
	: mFilePath(filepath), mRendererID(0), mUniformBlock(pUniformBlock)
{
	std::string p = std::filesystem::absolute(filepath).string();
	mFilePath = p;

	ShaderProgramSource source = Shader::ParseShader(mFilePath);
	mRendererID = CreateShader(source.VertexSource, source.FragmentSource);
	mValid = true;

	// Set Uniform Block, used for global values such as Proj, View  matrices etc..
	bindShader();
	SetUniform1i(pUniformBlockStr.c_str(), mUniformBlock);
	unbindShader();

}

Shader::~Shader()
{
	printf("\nDeleting Shader %s\n", mFilePath.c_str());
	if (mValid)
	{
		GLCall(glDeleteProgram(mRendererID));
	}
}

void Shader::bindShader()
{
	if (mValid != true)
	{
		LOG_ERROR("Cannot Bind inactive Shader! Check compilation status. {}", mFilePath);
		return;
	}
	GLCall(glUseProgram(mRendererID));
	mBinded = true;
}

void Shader::unbindShader()
{
	if (mValid != true)
	{
		LOG_ERROR("Cannot UnBind inactive Shader! Check compilation status. {}", mFilePath);
		return;
	}

	GLCall(glUseProgram(0));
	mBinded = false;
}

void Shader::Reload()
{
	LOG_DEBUG("Reloading Shader {}", mFilePath.c_str());

	mValid = false;
	mUniformLocationCache.clear(); // Remove ID location cache.
	unsigned int delete_id = mRendererID; 
	ShaderProgramSource source = Shader::ParseShader(mFilePath);
	mRendererID = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glDeleteProgram(delete_id)); // Cleanup
	mValid = true;
}



void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1i(const std::string& name, bool value)
{
	GLCall(glUniform1i(GetUniformLocation(name), (int)value));
}

void Shader::SetUniform1i(const std::string& name, unsigned int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), (int)value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniform3f(const std::string& name, glm::vec3 value)
{
	GLCall(glUniform3fv(GetUniformLocation(name), 1, &value[0]));
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	GLCall(glUniform3f(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{	
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}


void Shader::SetUniform4f(const std::string& name, glm::vec4 value)
{
	GLCall(glUniform4fv(GetUniformLocation(name), 1, &value[0]));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

GLint Shader::GetUniformLocation(const std::string& name)
{
	if (!mBinded) 
	{
		LOG_ERROR("Cannot set uniform. Shader is not Binded! {}", mFilePath);
	}

	if (!mValid)
	{
		// Shader is not active, return false location.
		LOG_ERROR("Cannot set uniform. Shader is not Valid! {}", mFilePath);
		return -1;
	}

	if (mUniformLocationCache.find(name) != mUniformLocationCache.end())
	{
		return mUniformLocationCache[name];
	}

	GLCall(GLint location = glGetUniformLocation(mRendererID, name.c_str()));
	if (location == -1)
	{
		LOG_WARNING("Warning! Uniform {} doesnt exist for this shader {}!", name, mFilePath);
	}

	mUniformLocationCache[name] = location;
	return location;

}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{


	LOG_INFO("Looking for shader file {}", filepath.c_str());

	//Check if file exists!
	
	FILE* pFile;
	errno_t err;
	if ((err = fopen_s(&pFile, filepath.c_str(), "r")) != 0)
	{
		LOG_ERROR("\nCould not find shader! Check file path! {}", filepath.c_str());
	}
	else 
	{
		fclose(pFile);
		
	}



	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[2];

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	int lineNumber = 0;

	while (getline(stream, line))
	{
		lineNumber++;
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
				mVertexBegin = lineNumber;
			}


			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
				mFragmentBegin = lineNumber;
			}

		}

		else
		{
			ss[int(type)] << line << "\n";
		}

	}
	return { ss[0].str(), ss[1].str() };


}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);


	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);


		// Retrieve the line number from the error message.
		// Assuming first set of parentheses contains the line number. 

		bool collectStrValues = false;
		std::string lineStr = "";

		for (int i = 0; i < length; i++)
		{
			if (message[i] == ')') break;
			if (collectStrValues) lineStr.push_back(message[i]);
			if (message[i] == '(') collectStrValues = true;

		}

		int lineNumber = std::stoi(lineStr);
		lineNumber = (type == GL_VERTEX_SHADER ? lineNumber + mVertexBegin : lineNumber + mFragmentBegin);
		LOG_ERROR("\nShader Path: {}\nType: {}\nFailed to compile line: {}\nError Message:  {}", 
			      mFilePath.c_str(), 
			     (type == GL_VERTEX_SHADER ? "Vertex" : "fragment"), 
			      lineNumber, \
			      message);


		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;

}
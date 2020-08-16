#include "Shader.h"


Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = Shader::ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);	
	m_Active = true;
}

Shader::Shader()
{
	LOG_WARNING("Shader Not Initialized!");
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
	GLCall(glUseProgram(0));
}

void Shader::Reload()
{
	m_Active = false;
	m_UniformLocationCache.clear(); // Remove ID location cache.
	unsigned int delete_id = m_RendererID; 
	ShaderProgramSource source = Shader::ParseShader(m_FilePath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glDeleteProgram(delete_id)); // Cleanup
	m_Active = true;

}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
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
	if (!m_Active)
	{
		// Shader is not active, return false location.
		return -1;
	}


	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}

	GLCall(GLint location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
	{
		LOG_WARNING("Warning! Uniform {} doesnt exist for this shader {}!", name, m_FilePath);
		//std::cout << "Warning: uniform " << name << "" << " doesn't exist" << std::endl;
	}

	m_UniformLocationCache[name] = location;
	return location;

}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	//Check if file exists!
	if (FILE *file = fopen(filepath.c_str(), "r"))
	{
		fclose(file);
	}
	else 
	{
		LOG_ERROR("\nCould not find shader! Check file path! {}", filepath);
	}



	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[2];

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{

		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;

			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
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
		LOG_ERROR("Failed to compile {} {}", (type == GL_VERTEX_SHADER ? "Vertex" : "fragment"), message);
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
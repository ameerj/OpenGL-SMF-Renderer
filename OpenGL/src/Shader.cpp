#include "Shader.h"
#include "Renderer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0) {
	ShaderProgram source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource, source.ComputeSource);

}
Shader::~Shader() {
	GLFunc(glDeleteProgram(m_RendererID));
}

ShaderProgram Shader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	std::string line;
	std::stringstream ss[3];

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1, COMPUTE = 2,
	};
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
			else if (line.find("compute") != std::string::npos)
				type = ShaderType::COMPUTE;
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(), ss[1].str(), ss[2].str() };
}

unsigned int Shader::CompileShader(const std::string& source, unsigned int type) {
	// unsigned int is the C++ type of the opengl types.
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLFunc(glShaderSource(id, 1, &src, nullptr));
	GLFunc(glCompileShader(id));

	// Error handling
	int res;
	GLFunc(glGetShaderiv(id, GL_COMPILE_STATUS, &res));
	if (res == GL_FALSE) {
		int length;
		GLFunc(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLFunc(glGetShaderInfoLog(id, length, &length, message));
		std::cout << message << std::endl;
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& computeShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);
	if (computeShader != "") {
	unsigned int cs = CompileShader(computeShader, GL_COMPUTE_SHADER);

		GLFunc(glAttachShader(program, cs));
		GLFunc(glLinkProgram(program));
		GLFunc(glDeleteShader(cs));
		return program;
	}
	GLFunc(glAttachShader(program, vs));
	GLFunc(glAttachShader(program, fs));

	GLFunc(glLinkProgram(program));
	GLFunc(glValidateProgram(program));

	GLFunc(glDeleteShader(vs));
	GLFunc(glDeleteShader(fs));

	return program;
}

void Shader::Bind() const {
	GLFunc(glUseProgram(m_RendererID));

}
void Shader::Unbind() const {
	GLFunc(glUseProgram(0));

}

void Shader::SetUniform1i(const std::string& name, int value) {
	GLFunc(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	GLFunc(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform3u(const std::string & name, uint32_t v0, uint32_t v1, uint32_t v2) {
	GLFunc(glUniform3ui(GetUniformLocation(name), v0, v1, v2));
}

void Shader::SetUniform2u(const std::string & name, uint32_t v0, uint32_t v1) {
	GLFunc(glUniform2ui(GetUniformLocation(name), v0, v1));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4 matrix) {
	GLFunc(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}


unsigned int Shader::GetUniformLocation(const std::string& name) {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	GLFunc(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		std::cout << "Warning: uniform '" << name << "; doesn't exist" << std::endl;
	m_UniformLocationCache[name] = location;
	return location;
}

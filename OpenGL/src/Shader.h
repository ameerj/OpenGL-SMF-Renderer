#pragma once
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct ShaderProgram {
	std::string VertexSource;
	std::string FragmentSource;
	std::string ComputeSource;
};

class Shader {
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform3u(const std::string& name, uint32_t v0, uint32_t v1, uint32_t v2);
	void SetUniform2u(const std::string& name, uint32_t v0, uint32_t v1);
private:
	unsigned int GetUniformLocation(const  std::string& name);
	//bool CompileShader();
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& computeShader);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	ShaderProgram ParseShader(const std::string& filepath);
};

#include "Renderer.h"
#include <iostream>

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLCheckError(const char* func, const char* file, int line) {
	while (GLenum err = glGetError()) {
		std::cout << "[OpenGL Error] (" << err << "): " <<
			func << " " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}

void Renderer::Clear() {
	GLFunc(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLFunc(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

#pragma once
#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

// Define GL Error checking macros! msvc specific.
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLFunc(x) GLClearError(); x; ASSERT(GLCheckError(#x, __FILE__, __LINE__))

void GLClearError();
bool GLCheckError(const char* func, const char* file, int line);

class Renderer {
public: 
	void Clear();
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};
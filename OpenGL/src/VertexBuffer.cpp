#include "VertexBuffer.h"
#include "Renderer.h"
#include <iostream>


VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
	GLFunc(glGenBuffers(1, &m_RendererID));
	GLFunc(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLFunc(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}
VertexBuffer::~VertexBuffer(){
	GLFunc(glDeleteBuffers(1, &m_RendererID));
	std::cout << "DESTROYED VB" << std::endl;

}

void VertexBuffer::Bind() const {
	GLFunc(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}
void VertexBuffer::Unbind() const {
	GLFunc(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

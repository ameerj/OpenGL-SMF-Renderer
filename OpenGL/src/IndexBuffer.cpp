#include "IndexBuffer.h"
#include "Renderer.h"


IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {
	GLFunc(glGenBuffers(1, &m_RendererID));
	GLFunc(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	GLFunc(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}
IndexBuffer::~IndexBuffer(){
	GLFunc(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Bind() const {
	GLFunc(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
void IndexBuffer::Unbind() const {
	GLFunc(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

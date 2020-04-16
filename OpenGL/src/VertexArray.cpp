#include "VertexArray.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include <iostream>

VertexArray::VertexArray() {
	GLFunc(glGenVertexArrays(1, &m_RendererID));
}
VertexArray::~VertexArray() {
	GLFunc(glDeleteVertexArrays(1, &m_RendererID));
	std::cout << "DESTROYED VA" << std::endl;

}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	Bind();
	vb.Bind();
	unsigned int offset = 0;
	const auto& elements = layout.GetElements();
	for (unsigned int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLFunc(glEnableVertexAttribArray(i));
		GLFunc(glVertexAttribPointer(i, element.count, element.type, element.normalized,
			layout.GetStride(), (const void*) offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const {
	GLFunc(glBindVertexArray(m_RendererID));
}
void VertexArray::Unbind() const {
	GLFunc(glBindVertexArray(0));
}

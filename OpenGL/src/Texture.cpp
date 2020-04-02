#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path)
	: m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0) {
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GLFunc(glGenTextures(1, &m_RendererID));
	GLFunc(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLFunc(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLFunc(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}
Texture::~Texture() {
	GLFunc(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) {
	GLFunc(glActiveTexture(GL_TEXTURE0 + slot));
	GLFunc(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}
void Texture::Unbind() {
	GLFunc(glBindTexture(GL_TEXTURE_2D, 0));
}


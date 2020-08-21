#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "astc_decode.h"
#include "astc.h"

#define 	GLFW_VERSION_MAJOR   4
#define 	GLFW_VERSION_MINOR   3

// 
// class InputBitStream {
// public:
// 	constexpr explicit InputBitStream(const u8* ptr, std::size_t start_offset = 0)
// 		: cur_byte{ ptr }, next_bit{ start_offset % 8 } {}
// 
// 	constexpr std::size_t GetBitsRead() const {
// 		return bits_read;
// 	}
// 
// 	constexpr bool ReadBit() {
// 		const bool bit = (*cur_byte >> next_bit++) & 1;
// 		while (next_bit >= 8) {
// 			next_bit -= 8;
// 			cur_byte++;
// 		}
// 
// 		bits_read++;
// 		return bit;
// 	}
// 
// 	constexpr u32 ReadBits(std::size_t nBits) {
// 		u32 ret = 0;
// 		for (std::size_t i = 0; i < nBits; ++i) {
// 			ret |= (ReadBit() & 1) << i;
// 		}
// 		return ret;
// 	}
// 
// 	template <std::size_t nBits>
// 	constexpr u32 ReadBits() {
// 		u32 ret = 0;
// 		for (std::size_t i = 0; i < nBits; ++i) {
// 			ret |= (ReadBit() & 1) << i;
// 		}
// 		return ret;
// 	}
// 
// private:
// 	const u8* cur_byte;
// 	std::size_t next_bit = 0;
// 	std::size_t bits_read = 0;
// };
// 

// std::array EncodingsValues = MakeEncodedValues();
void GLAPIENTRY
MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
	GLsizei length, const GLchar* message, const void* userParam) {
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

int main(void){
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(512, 512, "OGL ASTC", NULL, NULL);
	if (!window){
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	std::cout << "VERSION " <<  glfwGetVersionString() << std::endl;
	std::cout << "OpenGL version: " << glGetString(GL_VERSION)
		<< ", renderer: " << glGetString(GL_RENDERER) << std::endl;

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if (major * 10 + minor < 43) {
		throw std::runtime_error(
			"OpenGL version 4.3 or higher is required to run this program.");
	}
	if (glewInit() != GLEW_OK)
		std::cout << "Error: Glew did not init :(" << std::endl;


	// std::ifstream instream("res/textures/biggdog.astc", std::ios::in | std::ios::binary);
	std::ifstream instream("res/textures/patterninput.astc", std::ios::in | std::ios::binary);
	std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
	std::vector<uint32_t> image;

	u32 width, height, depth, block_width, block_height;

	width = height = 256;
	depth = 1;
	block_height = block_width = 6;

	std::vector<uint8_t> test = Tegra::Texture::ASTC::Decompress(data.data(), width, height, depth, block_width, block_height);
	//InputBitStream stm(data.data()) ;
	//u32 res = stm.ReadBits<32>();

	

	for (std::size_t i = 0; i < data.size(); i+=4) {
		uint32_t img_32;
		img_32 =  (data[i] &     (0xff)) << 24;
		img_32 |= (data[i + 1] & (0xff)) << 16;
		img_32 |= (data[i + 2] & (0xff)) << 8;
		img_32 |= (data[i + 3] & (0xff));

		image.push_back(img_32);
	}

	{

		// astc_compressed_image img{};
		// load_cimage("res/textures/biggdog.astc", img);

		// FILE * pFile;
		// long lSize;
		// size_t result;
		// 
		// fopen_s(&pFile, "res/textures/biggdog.astc", "rb");
		// // obtain file size:
		// fseek(pFile, 0, SEEK_END);
		// lSize = ftell(pFile);
		// rewind(pFile);

		// EncodingsValues[1];

		// // During init, enable debug output
		// glEnable(GL_DEBUG_OUTPUT);
		// glDebugMessageCallback(MessageCallback, 0);

		GLuint IMG_ssbo;
		GLFunc(glGenBuffers(1, &IMG_ssbo));
		GLFunc(glBindBuffer(GL_SHADER_STORAGE_BUFFER, IMG_ssbo));
		GLFunc(glBufferData(GL_SHADER_STORAGE_BUFFER, data.size(), data.data(), GL_STATIC_COPY));
		GLFunc(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, IMG_ssbo));
		GLFunc(glBindBuffer(GL_SHADER_STORAGE_BUFFER, IMG_ssbo)); // unbind

		GLuint ENC_ssbo;
		GLFunc(glGenBuffers(1, &ENC_ssbo));
		GLFunc(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ENC_ssbo));
		GLFunc(glBufferData(GL_SHADER_STORAGE_BUFFER, EncodingsValues.size() * sizeof(IntegerEncodedValue), EncodingsValues.data(), GL_STATIC_COPY));
		GLFunc(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ENC_ssbo));
		GLFunc(glBindBuffer(GL_SHADER_STORAGE_BUFFER, ENC_ssbo)); // unbind
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// dimensions of the image
		GLuint tex_output;
		GLFunc(glGenTextures(1, &tex_output));
		GLFunc(glActiveTexture(GL_TEXTURE0));
		GLFunc(glBindTexture(GL_TEXTURE_2D, tex_output));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLFunc(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT,
			NULL));
		GLFunc(glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F));

		Shader c_shader("res/shaders/compute.comp");
		c_shader.Bind();
		c_shader.SetUniform3u("image_size", (GLuint)width, (GLuint)height, 1);
		int num_blocks_x = static_cast<int>(std::ceil(float(width) / float(block_width)));
		int num_blocks_y = static_cast<int>(std::ceil(float(height) / float(block_height)));
		c_shader.SetUniform2u("num_image_blocks", num_blocks_x, num_blocks_y);
		c_shader.SetUniform2u("block_dims", block_width, block_height);
		GLFunc(glDispatchCompute((GLuint)num_blocks_x, (GLuint)num_blocks_y, 1));

		// make sure writing to image has finished before read
		GLFunc(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
		c_shader.Unbind();

		float positions[] = {
			-1, -1,  0.0f, 0.0f,
			1, -1, 1.0f, 0.0f,
			1, 1, 1.0f, 1.0f,
			-1, 1, 0.0f, 1.0f
		};
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		unsigned int vao; // vertex array buffer
		GLFunc(glGenVertexArrays(1, &vao));
		GLFunc(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(-1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::vec3 translationA = glm::vec3(0, 0, 0);
		//glm::vec3 translationB = glm::vec3(400, 200, 0);
		Shader shader("res/shaders/shader.shader");
		// 
		shader.Bind();
		//shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.7f, 1.0f);

		Renderer renderer;

		//Texture texture("res/textures/icon.png");
		// Texture texture("out.png");
		// texture.Bind(0);
		// shader.SetUniform1i("u_Texture", 0);
		float r = 0.2f;
		float increment = 0.005f;
		while (!glfwWindowShouldClose(window)) {
			renderer.Clear();
			shader.Bind();
			
			// object A
			glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
			glm::mat4 mvp = proj * view * model;
			// shader.SetUniform4f("u_Color", r, 0.3f, 1.0f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);
			

			if (r > 1.0f)
				increment = -0.005f;
			else if (r < 0.3f)
				increment = 0.005f;
			r += increment;

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}

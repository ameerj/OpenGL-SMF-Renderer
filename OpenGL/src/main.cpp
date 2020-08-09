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
#define 	GLFW_VERSION_MAJOR   4
#define 	GLFW_VERSION_MINOR   3

int main(void){
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
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
	{
		/*
		// updating to include texture coords
		float positions[] = {
			-50, -50,
			50, -50,
			50, 50,
			-50, 50,
		};
		*/

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


		// dimensions of the image
		int tex_w = 512, tex_h = 512;
		GLuint tex_output;
		GLFunc(glGenTextures(1, &tex_output));
		GLFunc(glActiveTexture(GL_TEXTURE0));
		GLFunc(glBindTexture(GL_TEXTURE_2D, tex_output));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLFunc(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLFunc(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
			NULL));
		GLFunc(glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F));

		Shader c_shader("res/shaders/compute.shader");
		c_shader.Bind();


		GLFunc(glDispatchCompute((GLuint)tex_w, (GLuint)tex_h, 1));

		// make sure writing to image has finished before read
		GLFunc(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		glm::vec3 translationA = glm::vec3(0, 0, 0);
		//glm::vec3 translationB = glm::vec3(400, 200, 0);
		Shader shader("res/shaders/shader.shader");
		// 
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.7f, 1.0f);

		Renderer renderer;
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		// Setup style
		ImGui::StyleColorsDark();

		//Texture texture("res/textures/icon.png");
		// Texture texture("out.png");
		// texture.Bind(0);
		// shader.SetUniform1i("u_Texture", 0);
		float r = 0.2f;
		float increment = 0.005f;
		while (!glfwWindowShouldClose(window)) {
			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();
			shader.Bind();
			
			// object A
			glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
			glm::mat4 mvp = proj * view * model;
			shader.SetUniform4f("u_Color", r, 0.3f, 1.0f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);
			
			// object B
			// model = glm::translate(glm::mat4(1.0f), translationB);
			// mvp = proj * view * model;
			// shader.SetUniform4f("u_Color", 0.0, r, 0.7f, 1.0f);
			// shader.SetUniformMat4f("u_MVP", mvp);

			// renderer.Draw(va, ib, shader);

			if (r > 1.0f)
				increment = -0.005f;
			else if (r < 0.3f)
				increment = 0.005f;
			r += increment;
			{
				ImGui::SliderFloat3("Translation", &translationA.x, 0.0f, 960.0f);
				//ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			glfwPollEvents();
		}
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

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
#include "SmfModel.h"


float aspectaxis() {
	int screen_width = 1024;
	int screen_height = 1024;
	float outputzoom = 1.0f;
	float aspectorigin = 16.0f / 9.0f;
	int aspectconstraint = 1;
	switch (aspectconstraint)
	{
	case 1:
		if ((screen_width / screen_height) < aspectorigin)
			outputzoom *= ((screen_width / screen_height) / aspectorigin);
		else
			outputzoom *= (aspectorigin / aspectorigin);
		break;
	case 2:
		outputzoom *= ((screen_width / screen_height) / aspectorigin);
		break;
	default:
		outputzoom *= (aspectorigin / aspectorigin);
	}
	return outputzoom;
}

float recalculatefov() {
	return 2.0f * glm::atan(glm::tan(glm::radians(45.0f / 2.0f)) / aspectaxis());
}


int main(void) {
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1024, 1024, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error: Glew did not init :(" << std::endl;
	{
		float cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			 1.0, -1.0,  1.0,
			 1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			 1.0, -1.0, -1.0,
			 1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0
		};
		unsigned int cube_elements[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// right
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// left
			4, 0, 3,
			3, 7, 4,
			// bottom
			4, 5, 1,
			1, 0, 4,
			// top
			3, 2, 6,
			6, 7, 3
		};
		SmfModel smf("res/smf/teapot.smf");
		unsigned int vao; // vertex array buffer
		GLFunc(glGenVertexArrays(1, &vao));
		GLFunc(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer cube_vb(smf.GetPositions().positions, smf.GetPositions().size * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		//layout.Push<float>(2);
		va.AddBuffer(cube_vb, layout);

		IndexBuffer ib(smf.GetFaces().faces, smf.GetFaces().count);
		// Transformation initialization
		glm::vec3 translationA = glm::vec3(0, 0, 0);
		glm::vec3 translationB = glm::vec3(0, 0, 0);
		glm::vec3 rotationA = glm::vec3(0, 45, 0);
		glm::vec3 axis_x(1, 0, 0);
		glm::vec3 axis_y(0, 1, 0);
		glm::vec3 axis_z(0, 0, 1);

		Shader shader("res/shaders/shader.shader");

		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.7f, 1.0f);

		Renderer renderer;
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		// Setup style
		ImGui::StyleColorsDark();

		//Texture texture("res/textures/icon.png");
		//texture.Bind(0);
		//shader.SetUniform1i("u_Texture", 0);
		GLFunc(glEnable(GL_DEPTH_TEST));
		GLFunc(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		float r = 0.2f;
		float increment = 0.005f;
		while (!glfwWindowShouldClose(window)) {
			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();
			shader.Bind();
			GLFunc(glEnable(GL_DEPTH_TEST));
			GLFunc(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			glm::mat4 projection = glm::perspective(recalculatefov(), 1.0f * 1024 / 1024, 0.1f, 10.0f);
			glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA + glm::vec3(0, 0, -4));
			glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationA.x), axis_x)
								* glm::rotate(glm::mat4(1.0f), glm::radians(rotationA.y), axis_y)
								* glm::rotate(glm::mat4(1.0f), glm::radians(rotationA.z), axis_z);
			glm::mat4 mvp = projection * view * model * rotation;
			shader.SetUniform4f("u_Color", r, 0.3f, 1.0f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);

			if (r > 1.0f)
				increment = -0.005f;
			else if (r < 0.3f)
				increment = 0.005f;
			r += increment;
			{
				ImGui::SliderFloat3("Translation", &translationA.x, -2.0f, 2.0f);
				ImGui::SliderFloat3("Rotation angle", &rotationA.x, 0.0f, 360.0f);
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

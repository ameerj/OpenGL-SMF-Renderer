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
	float aspectorigin = 1.0 / 1.0f;
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

float recalculatefov(float view_angle) {
	return 2.0f * glm::atan(glm::tan(glm::radians(view_angle / 2.0f)) / aspectaxis());
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
		SmfModel ModelA("res/smf/bunny_200.smf");
		SmfModel ModelB("res/smf/cube.smf");
		SmfModel sphere("res/smf/sphere.smf"); // sphere, 0.5 radius, centered at origin

		unsigned int vao; // vertex array buffer
		GLFunc(glGenVertexArrays(1, &vao));
		GLFunc(glBindVertexArray(vao));

		VertexArray va1;
		VertexBuffer vb1(ModelA.GetPositions(), ModelA.GetPositionSize() * sizeof(float));
		VertexBufferLayout layout1;
		layout1.Push<float>(3);
		//layout.Push<float>(2);
		va1.AddBuffer(vb1, layout1);
		IndexBuffer ib1(ModelA.GetFaces(), ModelA.GetFaceCount());
		
		// Object 2
		VertexArray va2;
		VertexBuffer vb2(ModelB.GetPositions(), ModelB.GetPositionSize() * sizeof(float));
		VertexBufferLayout layout2;
		layout2.Push<float>(3);
		//layout.Push<float>(2);
		va2.AddBuffer(vb2, layout2);
		IndexBuffer ib2(ModelB.GetFaces(), ModelB.GetFaceCount());

		// Object  sphere
		VertexArray vas;
		VertexBuffer vbs(sphere.GetPositions(), sphere.GetPositionSize() * sizeof(float));
		VertexBufferLayout layout_s;
		layout_s.Push<float>(3);
		//layout.Push<float>(2);
		vas.AddBuffer(vbs, layout_s);
		IndexBuffer ibs(sphere.GetFaces(), sphere.GetFaceCount());

		// Camera initializations
		float angle = 60.0f;
		glm::vec3 camera(0.0, 0.0, 3.0);

		// Transformation initialization
		float scaleA = 1.0f;
		glm::vec4 colorA(1.0f, 0.0f, 0.40f, 1.0f);
		glm::vec3 translationA(0, 0, 0);
		glm::vec3 rotationA(0, 0, 0);

		float scaleB = 0.75f;
		glm::vec4 colorB(0.0f, 1.0f, 0.40f, 1.0f);
		glm::vec3 translationB(-1, -1, 0);
		glm::vec3 rotationB(0, 0, 0);

		float scaleS = 1.0f;
		glm::vec4 colorS(1.0f, 1.0f, 1.0f, 1.0f);
		glm::vec3 translationS(1, 0, 0);
		glm::vec3 rotationS(0, 0, 0);

		glm::vec3 init_scale(1, 1, 1);
		glm::vec3 axis_x(1, 0, 0);
		glm::vec3 axis_y(0, 1, 0);
		glm::vec3 axis_z(0, 0, 1);

		Shader shader("res/shaders/shader.shader");

		shader.Bind();

		Renderer renderer;
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		// Setup style
		ImGui::StyleColorsDark();

		while (!glfwWindowShouldClose(window)) {
			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();
			shader.Bind();
			GLFunc(glEnable(GL_DEPTH_TEST));
			GLFunc(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			//GLFunc(glDepthFunc(GL_LESS));
			GLFunc(glDepthRange(-1, 1));

			glm::mat4 projection = glm::perspective(recalculatefov(angle), 1.0f * 1024 / 1024, 1.0f, -1.0f);
			glm::mat4 view = glm::lookAt(camera, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

			// Model A
			glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA + glm::vec3(0, 0, 0));
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationA.x), axis_x)
								* glm::rotate(glm::mat4(1.0f), glm::radians(rotationA.y), axis_y)
								* glm::rotate(glm::mat4(1.0f), glm::radians(rotationA.z), axis_z);
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), init_scale * scaleA);

			glm::mat4 mvp = projection * view * model * rotation * scale;
			shader.SetUniform4f("u_Color", colorA.r, colorA.g, colorA.b, colorA.a);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va1, ib1, shader);
			
			// Render Model B
			model = glm::translate(glm::mat4(1.0f), translationB + glm::vec3(0, 0, 0));
			rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationB.x), axis_x)
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotationB.y), axis_y)
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotationB.z), axis_z);
			scale = glm::scale(glm::mat4(1.0f), init_scale * scaleB);

			mvp = projection * view * model * rotation * scale;
			shader.SetUniform4f("u_Color", colorB.r, colorB.g, colorB.b, colorB.a);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va2, ib2, shader);

			// Render Sphere
			model = glm::translate(glm::mat4(1.0f), translationS + glm::vec3(0, 0, 0));
			rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotationS.x), axis_x)
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotationS.y), axis_y)
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotationS.z), axis_z);
			scale = glm::scale(glm::mat4(1.0f), init_scale * scaleS);

			mvp = projection * view * model * rotation * scale;
			shader.SetUniform4f("u_Color", colorS.r, colorS.g, colorS.b, colorS.a);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(vas, ibs, shader);

			{
				if (ImGui::CollapsingHeader("Camera")){
					ImGui::Text("Camera");
					ImGui::SliderFloat3("Camera Position", &camera.x, -4.0f, 4.0f);
					ImGui::SliderFloat("Camera FOV", &angle, 0.0f, 180.0f);
					if (ImGui::Button("Reset Camera")) {
						camera = glm::vec3(0.0, 0.0, 3.0);
						angle = 60.0f;
					}
				}

				if (ImGui::CollapsingHeader(ModelA.GetModelName().c_str())) {
					ImGui::SliderFloat4("Color A", &colorA.r, 0.0f, 1.0f);
					ImGui::SliderFloat3("Translation A", &translationA.x, -4.0f, 4.0f);
					ImGui::SliderFloat3("Rotation Angle A", &rotationA.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Scale A", &scaleA, 0.0f, 2.0f);
					if (ImGui::Button("Reset A")) {
						scaleA = 1.0f;
						translationA = glm::vec3(0, 0, 0);
						rotationA = glm::vec3(0, 0, 0);
					}
				}
				if (ImGui::CollapsingHeader(ModelB.GetModelName().c_str())) {
					ImGui::SliderFloat4("Color B", &colorB.r, 0.0f, 1.0f);
					ImGui::SliderFloat3("Translation B", &translationB.x, -4.0f, 4.0f);
					ImGui::SliderFloat3("Rotation Angle B", &rotationB.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Scale B", &scaleB, 0.0f, 2.0f);
					if (ImGui::Button("Reset B")) {
						scaleB = 0.75f;
						translationB = glm::vec3(-1, -1, 0);
						rotationB = glm::vec3(0, 0, 0);
					}
				}

				if (ImGui::CollapsingHeader(sphere.GetModelName().c_str())) {
					ImGui::SliderFloat4("Color S", &colorS.r, 0.0f, 1.0f);
					ImGui::SliderFloat3("Translation S", &translationS.x, -4.0f, 4.0f);
					ImGui::SliderFloat3("Rotation Angle S", &rotationS.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Diameter S", &scaleS, 0.0f, 2.0f);
					if (ImGui::Button("Reset S")) {
						scaleS = 1.0f;
						translationS = glm::vec3(1, 0, 0);
						rotationS = glm::vec3(0, 0, 0);
					}
				}

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

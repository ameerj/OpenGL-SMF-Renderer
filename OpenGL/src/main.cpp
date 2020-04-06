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

glm::mat4 get_MRS(SmfModel smf_model) {
	glm::vec3 init_scale(1, 1, 1);
	glm::vec3 axis_x(1, 0, 0);
	glm::vec3 axis_y(0, 1, 0);
	glm::vec3 axis_z(0, 0, 1);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), smf_model.translation + glm::vec3(0, 0, 0));
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(smf_model.rotation.x), axis_x)
		* glm::rotate(glm::mat4(1.0f), glm::radians(smf_model.rotation.y), axis_y)
		* glm::rotate(glm::mat4(1.0f), glm::radians(smf_model.rotation.z), axis_z);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), init_scale * smf_model.scale);

	glm::mat4 mrs = model * rotation * scale;

	return mrs;
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
		SmfModel modelA("res/smf/bunny_200.smf", 1.0f, 
			glm::vec4(1.0f, 0.0f, 0.40f, 1.0f), 
			glm::vec3(0, 0, 0)
		);

		SmfModel modelB("res/smf/cube.smf", 0.75f, 
			glm::vec4(0.0f, 1.0f, 0.40f, 1.0f), 
			glm::vec3(-1, -1, 0)
		);
		SmfModel sphere("res/smf/sphere.smf", 1.0f, 
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec3(1, 0, 0)
		); // sphere, 0.5 radius, centered at origin

		unsigned int vao; // vertex array buffer
		GLFunc(glGenVertexArrays(1, &vao));
		GLFunc(glBindVertexArray(vao));

		VertexArray va1;
		VertexBuffer vb1(modelA.GetPositions(), modelA.GetPositionSize() * sizeof(float));
		VertexBufferLayout layout1;
		layout1.Push<float>(3);
		va1.AddBuffer(vb1, layout1);
		IndexBuffer ib1(modelA.GetFaces(), modelA.GetFaceCount());
		
		// Object 2
		VertexArray va2;
		VertexBuffer vb2(modelB.GetPositions(), modelB.GetPositionSize() * sizeof(float));
		VertexBufferLayout layout2;
		layout2.Push<float>(3);
		va2.AddBuffer(vb2, layout2);
		IndexBuffer ib2(modelB.GetFaces(), modelB.GetFaceCount());

		// Object  sphere
		VertexArray vas;
		VertexBuffer vbs(sphere.GetPositions(), sphere.GetPositionSize() * sizeof(float));
		VertexBufferLayout layout_s;
		layout_s.Push<float>(3);
		vas.AddBuffer(vbs, layout_s);
		IndexBuffer ibs(sphere.GetFaces(), sphere.GetFaceCount());

		// Camera initializations
		float angle = 60.0f;
		glm::vec3 camera(0.0, 0.0, 3.0);

		Shader shader("res/shaders/shader.shader");
		Renderer renderer;
		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		while (!glfwWindowShouldClose(window)) {
			renderer.Clear();
			ImGui_ImplGlfwGL3_NewFrame();
			shader.Bind();
			GLFunc(glEnable(GL_DEPTH_TEST));
			GLFunc(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			GLFunc(glDepthRange(-1, 1));

			glm::mat4 projection = glm::perspective(recalculatefov(angle), 1.0f * 1024 / 1024, 1.0f, -1.0f);
			glm::mat4 view = glm::lookAt(camera, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 mrs;
			glm::mat4 mvp;
			// Model A
			mrs = get_MRS(modelA);
			mvp = projection * view * mrs;
			shader.SetUniformVec4f("u_Color", modelA.color);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va1, ib1, shader);
			
			// Render Model B
			mrs = get_MRS(modelB);
			mvp = projection * view * mrs;
			shader.SetUniformVec4f("u_Color", modelB.color);
			shader.SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(va2, ib2, shader);

			// Render Sphere
			mrs = get_MRS(sphere);
			mvp = projection * view * mrs;
			shader.SetUniformVec4f("u_Color", sphere.color);
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

				if (ImGui::CollapsingHeader(modelA.GetModelName().c_str())) {
					ImGui::SliderFloat4("Color A", &modelA.color.r, 0.0f, 1.0f);
					ImGui::SliderFloat3("Translation A", &modelA.translation.x, -4.0f, 4.0f);
					ImGui::SliderFloat3("Rotation Angle A", &modelA.rotation.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Scale A", &modelA.scale, 0.0f, 2.0f);
					if (ImGui::Button("Reset A")) {
						modelA.scale = 1.0f;
						modelA.translation = glm::vec3(0, 0, 0);
						modelA.rotation = glm::vec3(0, 0, 0);
					}
				}
				if (ImGui::CollapsingHeader(modelB.GetModelName().c_str())) {
					ImGui::SliderFloat4("Color B", &modelB.color.r, 0.0f, 1.0f);
					ImGui::SliderFloat3("Translation B", &modelB.translation.x, -4.0f, 4.0f);
					ImGui::SliderFloat3("Rotation Angle B", &modelB.rotation.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Scale B", &modelB.scale, 0.0f, 2.0f);
					if (ImGui::Button("Reset B")) {
						modelB.scale = 0.75f;
						modelB.translation = glm::vec3(-1, -1, 0);
						modelB.rotation = glm::vec3(0, 0, 0);
					}
				}

				if (ImGui::CollapsingHeader(sphere.GetModelName().c_str())) {
					ImGui::SliderFloat4("Color S", &sphere.color.r, 0.0f, 1.0f);
					ImGui::SliderFloat3("Translation S", &sphere.translation.x, -4.0f, 4.0f);
					ImGui::SliderFloat3("Rotation Angle S", &sphere.rotation.x, 0.0f, 360.0f);
					ImGui::SliderFloat("Diameter S", &sphere.scale, 0.0f, 2.0f);
					if (ImGui::Button("Reset S")) {
						sphere.scale = 1.0f;
						sphere.translation = glm::vec3(1, 0, 0);
						sphere.rotation = glm::vec3(0, 0, 0);
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

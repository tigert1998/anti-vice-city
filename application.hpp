#pragma once

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "camera.hpp"
#include "skybox.hpp"
#include "bounding_box.hpp"

// Please always use shared to run this program 

class Application {
private:
	int width = 800, height = 600;
	GLFWwindow* window;

	Camera camera = Camera(glm::vec3(0, 0, 3), 0, 0);

	glm::mat4 model_matrix;

	const std::vector<std::string> skybox_urls = {
		"resources/skybox/left.jpg", "resources/skybox/right.jpg",
  		"resources/skybox/top.jpg", "resources/skybox/bottom.jpg",
  		"resources/skybox/front.jpg", "resources/skybox/back.jpg"
	};

	static void CursorPosCallback(GLFWwindow *window, double x, double y);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static glm::mat4 GetProjectionMatrix();
	static void ProcessInput(GLFWwindow *window);
	static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

public:
	static Application shared;
	Application();
	void Run();

};

Application Application::shared = Application();

void Application::CursorPosCallback(GLFWwindow *window, double x, double y) {
	static double mouse_x, mouse_y;
	static bool entered = false;
	double new_x = x / shared.width - 0.5;
	double new_y = y / shared.height - 0.5;
	if (entered)	
		shared.camera.Rotate(mouse_x - new_x, mouse_y - new_y);
	mouse_x = new_x;
	mouse_y = new_y;
	entered = true;
}

void Application::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	using namespace glm;
	float mul = pow(1.1, yoffset);
	shared.model_matrix = scale(shared.model_matrix, vec3(mul, mul, mul));
}

void Application::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	shared.width = width;  
	shared.height = height;  
}  

glm::mat4 Application::GetProjectionMatrix() {
	using namespace glm;
	return perspective(radians(45.0f), 1.0f * shared.width / shared.height, 0.1f, 1000.0f);
}

void Application::ProcessInput(GLFWwindow *window) {
	static float current_time, last_time;
	current_time = glfwGetTime();
	float delta_time = current_time - last_time;
	last_time = current_time;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
       	shared.camera.Move(MoveDirectionType::FRONT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
       	shared.camera.Move(MoveDirectionType::BACK, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
       	shared.camera.Move(MoveDirectionType::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
       	shared.camera.Move(MoveDirectionType::RIGHT, delta_time);
}

Application::Application() {
	using namespace glm;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Anti-vice City", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	model_matrix = scale(rotate(mat4(1), (float)M_PI / 2, vec3(1, 0, 0)), vec3(0.001, 0.001, 0.001));
}

void Application::Run() {
	using namespace glm;

	Skybox skybox_model = Skybox(skybox_urls);
	Shader skybox_shader = Shader("shaders/skybox.vs", "shaders/skybox.fs");

	Model city_model = Model("resources/models/city", "city.obj");
	Shader city_shader = Shader("shaders/city.vs", "shaders/city.fs");


	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox_shader.Use();
		skybox_shader.SetUniform<mat4>("view", mat4(mat3(camera.GetViewMatrix())));
		skybox_shader.SetUniform<mat4>("projection", GetProjectionMatrix());
		skybox_shader.SetUniform<mat4>("rotate", rotate(mat4(1), -(float)M_PI / 2, vec3(1, 0, 0)));

		skybox_model.Draw(skybox_shader);

		city_shader.Use();
		city_shader.SetUniform<mat4>("model", model_matrix);
		city_shader.SetUniform<mat4>("view", camera.GetViewMatrix());
		city_shader.SetUniform<mat4>("projection", GetProjectionMatrix());
		
		city_shader.SetUniform<vec3>("light.position", vec3(200, 200, 500));
		city_shader.SetUniform<vec3>("light.ambient", vec3(0.6, 0.6, 0.6));
		city_shader.SetUniform<vec3>("light.diffuse", vec3(1, 1, 1));
		city_shader.SetUniform<vec3>("light.specular", vec3(1, 1, 1));

		city_shader.SetUniform<vec3>("view_position", camera.position());
		city_shader.SetUniform<float>("material.shininess", 32);

		city_model.Draw(city_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
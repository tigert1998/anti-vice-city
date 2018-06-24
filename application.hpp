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
#include "car.hpp"

// Please always use shared to run this program 

class Application {
private:
	int width = 800, height = 600;
	GLFWwindow* window;

	Skybox *skybox_ptr;
	Model *car_model_ptr;
	Model *city_model_ptr;

	Shader *skybox_shader_ptr;
	Shader *car_shader_ptr;
	Shader *city_shader_ptr;

	Camera* camera_ptr;
	Car *car_ptr;

	glm::mat4 model_matrix;

	const std::vector<std::string> skybox_urls = {
		"resources/skybox/left.jpg", "resources/skybox/right.jpg",
  		"resources/skybox/top.jpg", "resources/skybox/bottom.jpg",
  		"resources/skybox/front.jpg", "resources/skybox/back.jpg"
	};

	static void CursorPosCallback(GLFWwindow *window, double x, double y);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
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
		shared.camera_ptr->Rotate(mouse_x - new_x, mouse_y - new_y);
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
	shared.camera_ptr->set_width_height_ratio(1.0f * width / height);
}  

void Application::ProcessInput(GLFWwindow *window) {
	static float current_time, last_time;
	current_time = glfwGetTime();
	float delta_time = current_time - last_time;
	last_time = current_time;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
       	shared.car_ptr->Move(MoveDirectionType::FRONT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
       	shared.car_ptr->Move(MoveDirectionType::BACK, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
       	shared.car_ptr->Move(MoveDirectionType::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
       	shared.car_ptr->Move(MoveDirectionType::RIGHT, delta_time);
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

	camera_ptr = new Camera(glm::vec3(0, 0, 3), 0, 0, 1.0f * width / height);

	skybox_ptr = new Skybox(skybox_urls);
	skybox_shader_ptr = new Shader("shaders/skybox.vs", "shaders/skybox.fs");

	city_model_ptr = new Model("resources/models/city", "city.obj");
	city_shader_ptr = new Shader("shaders/city.vs", "shaders/city.fs");

	car_model_ptr = new Model("resources/models/car", "car.obj");
	car_shader_ptr = new Shader("shaders/car.vs", "shaders/car.fs");
	car_ptr = new Car(*car_model_ptr, *car_shader_ptr, camera_ptr, vec3(10, 10, 0.5));

	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// skybox

		skybox_shader_ptr->Use();
		skybox_shader_ptr->SetUniform<mat4>("view", mat4(mat3(camera_ptr->GetViewMatrix())));
		skybox_shader_ptr->SetUniform<mat4>("projection", camera_ptr->GetProjectionMatrix());
		skybox_shader_ptr->SetUniform<mat4>("rotate", rotate(mat4(1), -(float)M_PI / 2, vec3(1, 0, 0)));

		skybox_ptr->Draw(*skybox_shader_ptr);

		// city

		city_shader_ptr->Use();
		city_shader_ptr->SetUniform<mat4>("model", model_matrix);
		city_shader_ptr->SetUniform<mat4>("view", camera_ptr->GetViewMatrix());
		city_shader_ptr->SetUniform<mat4>("projection", camera_ptr->GetProjectionMatrix());
		
		city_shader_ptr->SetUniform<vec3>("light.position", vec3(200, 200, 500));
		city_shader_ptr->SetUniform<vec3>("light.ambient", vec3(0.6, 0.6, 0.6));
		city_shader_ptr->SetUniform<vec3>("light.diffuse", vec3(1, 1, 1));
		city_shader_ptr->SetUniform<vec3>("light.specular", vec3(1, 1, 1));

		city_shader_ptr->SetUniform<vec3>("view_position", camera_ptr->position());
		city_shader_ptr->SetUniform<float>("material.shininess", 32);

		city_model_ptr->Draw(*city_shader_ptr);

		// car
		car_ptr->Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
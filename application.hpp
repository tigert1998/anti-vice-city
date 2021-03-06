#pragma once

#define GL_GLEXT_PROTOTYPES

#ifdef DEBUG
#include <iostream>
#endif

#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "skybox.hpp"
#include "world.hpp"
#include "car.hpp"

// Please always use shared to run this program 

class Application {
private:
	int width = 800, height = 600;
	GLFWwindow* window;

	Skybox *skybox_ptr;
	Camera* camera_ptr;
	Car *car_ptr;
	World *world_ptr;

	const std::vector<std::string> skybox_urls = {
		"resources/skybox/left.jpg", "resources/skybox/right.jpg",
  		"resources/skybox/top.jpg", "resources/skybox/bottom.jpg",
  		"resources/skybox/front.jpg", "resources/skybox/back.jpg"
	};

	static bool keys_pressed[1024];

	static void CursorPosCallback(GLFWwindow *window, double x, double y);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void ProcessInput(GLFWwindow *window);
	static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

public:
	static Application shared;
	Application();
	void Run();

};

bool Application::keys_pressed[1024] = {};

Application Application::shared = Application();

void Application::CursorPosCallback(GLFWwindow *window, double x, double y) {
	static double mouse_x, mouse_y;
	static bool entered = false;
	double new_x = x / shared.width - 0.5;
	double new_y = y / shared.height - 0.5;
	if (entered) {
		shared.camera_ptr->Rotate(mouse_x - new_x, mouse_y - new_y);
		shared.car_ptr->Rotate(mouse_x - new_x);
		shared.car_ptr->CameraAccompany();
	}
	mouse_x = new_x;
	mouse_y = new_y;
	entered = true;
}

void Application::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	shared.width = width;
	shared.height = height;
	shared.camera_ptr->set_width_height_ratio(1.0f * width / height);
}

void Application::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    else
    	keys_pressed[key] = action != GLFW_RELEASE;
}

void Application::ProcessInput(GLFWwindow *window) {
	static float current_time, last_time;
	current_time = glfwGetTime();
	float delta_time = current_time - last_time;
	last_time = current_time;
	if (keys_pressed[GLFW_KEY_W]) 
       	shared.car_ptr->Move(MoveDirectionType::FRONT, delta_time);
    if (keys_pressed[GLFW_KEY_S])
       	shared.car_ptr->Move(MoveDirectionType::BACK, delta_time);
    if (keys_pressed[GLFW_KEY_A])
       	shared.car_ptr->Move(MoveDirectionType::LEFT, delta_time);
    if (keys_pressed[GLFW_KEY_D])
       	shared.car_ptr->Move(MoveDirectionType::RIGHT, delta_time);

#ifdef DEBUG
	if (keys_pressed[GLFW_KEY_SPACE]) {
		shared.car_ptr->ShowPosition();
	}
#endif
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
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetKeyCallback(window, KeyCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
}

void Application::Run() {
	using namespace glm;
	using namespace std;

	camera_ptr = new Camera(glm::vec3(0, 0, 3), 0, 0, 1.0f * width / height);

	Shader *skybox_shader_ptr = new Shader("shaders/skybox.vs", "shaders/skybox.fs");
	skybox_ptr = new Skybox(skybox_urls, *skybox_shader_ptr, *camera_ptr);

	Model *world_model_ptr = new Model("resources/models/world", "world.obj", false);
	Shader *world_shader_ptr = new Shader("shaders/world.vs", "shaders/world.fs");
	world_ptr = new World(*world_model_ptr, *world_shader_ptr, *camera_ptr);

	Model *car_model_ptr = new Model("resources/models/car", "tank_tigher.obj", true);
	Shader *car_shader_ptr = new Shader("shaders/car.vs", "shaders/car.fs");
	car_ptr = new Car(*car_model_ptr, *car_shader_ptr, *camera_ptr, vec3(8.31, 8.01, 4.88));
	// car_ptr = new Car(*car_model_ptr, *car_shader_ptr, *camera_ptr, vec3(8.31, 8.01, 3.18));

	float last_time = 0.0f, current_time = 0.0f;
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox_ptr->Draw();
		world_ptr->Draw();
		car_ptr->Draw();

		current_time = glfwGetTime();
		float delta_time = current_time - last_time;
		last_time = current_time;
		car_ptr->Update(delta_time);


		if (car_model_ptr->Conflict(*world_model_ptr, car_ptr->model_matrix(), world_ptr->model_matrix())) {
			car_ptr->Disable();
		} else {
			car_ptr->Enable();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
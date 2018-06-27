#pragma once

#define GL_GLEXT_PROTOTYPES

#ifdef DEBUG
#include <iostream>
#endif

#include <algorithm>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shadow.hpp"
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

	Shadow *shadow_ptr;

	static bool keys_pressed[1024];

	static void CursorPosCallback(GLFWwindow *window, double x, double y);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void ProcessInput(GLFWwindow *window);
	static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);

	void InitShadows();
	void DisplayShadowTexture();

public:
	static Application shared;
	Application();
	void Run();

};

bool Application::keys_pressed[1024] = {};

Application Application::shared = Application();

void Application::InitShadows() {
	using namespace glm;
	static Shader shadow_shader("shaders/shadow.vs", "shaders/shadow.fs");
	static vec3 center_position = vec3(-3, 26.16, 4.9);
	static vec3 light_position = vec3(-21.26, 11.47, 6.42);
	static mat4 projection_matrix = ortho(0.0f, 2.55f, -4.7014f, 4.7014f, 1.0f, 20.0f);
	static mat4 view_matrix = lookAt(light_position, center_position, vec3(0, 0, 1));
	
	shadow_ptr = new Shadow(shadow_shader, view_matrix, projection_matrix);
}

void Application::DisplayShadowTexture() {
	using namespace glm;
	using namespace std;
	static bool is_first_enter = true;
	static uint32_t vao, vbo;
	static vector<float> quad_vertices = {
		// Positions   Texture Coords
		-1, 1.0f, 0.0f,  0.0f, 1.0f,
		-1, -1, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1, 0.0f, 1.0f, 0.0f
	};
	static Shader test_shadow_shader("shaders/test_shadow.vs", "shaders/test_shadow.fs");

	if (is_first_enter) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, quad_vertices.size() * sizeof(float), quad_vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 5 * sizeof(float), (void *) 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void *) (3 * sizeof(float)));
		glBindVertexArray(0);

		is_first_enter = false;
	}

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadow_ptr->texture());
	test_shadow_shader.Use();
	test_shadow_shader.SetUniform<int32_t>("shadow_texture", 0); 
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

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
    if (keys_pressed[GLFW_KEY_J])
       	shared.car_ptr->Move(MoveDirectionType::DOWN, delta_time);
    if (keys_pressed[GLFW_KEY_K])
       	shared.car_ptr->Move(MoveDirectionType::UP, delta_time);
}

Application::Application() {
	using namespace glm;
	using namespace std;
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

	const vector<string> skybox_urls = {
		"resources/skybox/left.jpg", "resources/skybox/right.jpg",
  		"resources/skybox/top.jpg", "resources/skybox/bottom.jpg",
  		"resources/skybox/front.jpg", "resources/skybox/back.jpg"
	};

	camera_ptr = new Camera(vec3(0, 0, 3), 0, 0, 1.0f * width / height);

	Shader *skybox_shader_ptr = new Shader("shaders/skybox.vs", "shaders/skybox.fs");
	skybox_ptr = new Skybox(skybox_urls, *skybox_shader_ptr, *camera_ptr);

	Model *world_model_ptr = new Model("resources/models/world", "world.obj", false);
	Shader *world_shader_ptr = new Shader("shaders/world.vs", "shaders/world.fs");
	world_ptr = new World(*world_model_ptr, *world_shader_ptr, *camera_ptr);

	Model *car_model_ptr = new Model("resources/models/car", "tank_tigher.obj", true);
	Shader *car_shader_ptr = new Shader("shaders/car.vs", "shaders/car.fs");
	car_ptr = new Car(*car_model_ptr, *car_shader_ptr, *camera_ptr, vec3(8.31, 8.01, 4.84));

	InitShadows();
}

void Application::Run() {
	using namespace glm;
	using namespace std;

	glClearColor(0, 0, 0, 0);
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		shadow_ptr->Render({
			&car_ptr->model(), 
			&world_ptr->model()
		}, {
			car_ptr->model_matrix(),
			world_ptr->model_matrix()
		});

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadow_ptr->SetShaders({
			&car_ptr->shader(),
			&world_ptr->shader()
		});

		skybox_ptr->Draw();
		world_ptr->Draw();
		car_ptr->Draw();

		if (car_ptr->model().Conflict(world_ptr->model(), car_ptr->model_matrix(), world_ptr->model_matrix())) {
			car_ptr->Disable();
		} else {
			car_ptr->Enable();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
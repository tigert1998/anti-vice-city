#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "camera.hpp"

#include "bounding_box.hpp"

using namespace std;
using namespace glm;

int width = 800, height = 600;

Camera camera(vec3(0, 0, 3), 0, 0);
mat4 model_matrix, projection_matrix;

void CursorPosCallback(GLFWwindow *window, double x, double y) {
	static double mouse_x, mouse_y;
	static bool entered = false;
	double new_x = x / width - 0.5;
	double new_y = y / height - 0.5;
	if (entered)	
		camera.Rotate(mouse_x - new_x, mouse_y - new_y);
	mouse_x = new_x;
	mouse_y = new_y;
	entered = true;
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	float mul = pow(1.1, yoffset);
	model_matrix = scale(model_matrix, vec3(mul, mul, mul));
}

void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
	::width = width;  
	::height = height;  
}  

mat4 GetProjectionMatrix() {
	return perspective(radians(45.0f), 1.0f * width / height, 0.1f, 1000.0f);
}

void ProcessInput(GLFWwindow *window) {
	static float current_time, last_time;
	current_time = glfwGetTime();
	float delta_time = current_time - last_time;
	last_time = current_time;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
       	camera.Move(MoveDirectionType::FRONT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
       	camera.Move(MoveDirectionType::BACK, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
       	camera.Move(MoveDirectionType::LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
       	camera.Move(MoveDirectionType::RIGHT, delta_time);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Batmobile", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	Model model("resources/models/city", "city.obj");
	Shader shader("shaders/city.vs", "shaders/city.fs");

	auto rotate_matrix = glm::rotate(mat4(1), 0.5f, vec3(1, 0, 0));
	model_matrix = scale(rotate(mat4(1), (float)M_PI / 2, vec3(1, 0, 0)), vec3(0.001, 0.001, 0.001));

	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		shader.SetUniform<mat4>("model", model_matrix);
		shader.SetUniform<mat4>("view", camera.GetViewMatrix());
		shader.SetUniform<mat4>("projection", GetProjectionMatrix());
		
		shader.SetUniform<vec3>("light.position", vec3(200, 200, 500));
		shader.SetUniform<vec3>("light.ambient", vec3(0.6, 0.6, 0.6));
		shader.SetUniform<vec3>("light.diffuse", vec3(1, 1, 1));
		shader.SetUniform<vec3>("light.specular", vec3(1, 1, 1));

		shader.SetUniform<vec3>("view_position", camera.position());
		shader.SetUniform<float>("material.shininess", 32);

		model.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

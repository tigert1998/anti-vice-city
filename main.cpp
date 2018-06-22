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

bool pressed = false;

Camera camera(vec3(0, 0, 3), 0, 0);
mat4 model_matrix, projection_matrix;

void CursorPosCallback(GLFWwindow *window, double x, double y) {
	static double mouse_x, mouse_y;
	double new_x = x / width - 0.5;
	double new_y = y / height - 0.5;
	static double theta = 0, gamma = 0;	
	if (pressed) 
		camera.Rotate(new_x - mouse_x, new_y - mouse_y);
	mouse_x = new_x;
	mouse_y = new_y;
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_LEFT) return;
	pressed = action == GLFW_PRESS;
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

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Batmobile", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

	glEnable(GL_DEPTH_TEST);

	Model model("resources/models/city", "Nimbasa City.obj");
	Shader shader("shaders/batmobile.vs", "shaders/batmobile.fs");

	auto rotate_matrix = glm::rotate(mat4(1), 0.5f, vec3(1, 0, 0));
	model_matrix = scale(rotate(mat4(1), (float)M_PI / 2, vec3(1, 0, 0)), vec3(0.001, 0.001, 0.001));

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

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

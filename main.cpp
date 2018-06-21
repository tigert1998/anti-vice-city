#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "camera.hpp"

using namespace std;
using namespace glm;

const int width = 800, height = 600;

bool pressed = false;
vec3 center_position = vec3(0.0f, 0.0f, 0.0f);

Camera camera(3);

mat4 ViewProjection(vec3 camera_position, vec3 center_position) {
	float aspect = static_cast<float>(width) / height;
	mat4 projection = perspective(radians(45.0f), aspect, 0.1f, 1000.0f);
	mat4 view = lookAt(camera_position, center_position, vec3(0, 0, 1));
	return projection * view;
}

void CursorPosCallback(GLFWwindow *window, double x, double y) {
	static double mouse_x, mouse_y;
	double new_x = x / width - 0.5;
	double new_y = y / height - 0.5;

	static double theta = 0, gamma = 0;	
	if (pressed) {
		camera.Rotate(- new_x + mouse_x, new_y - mouse_y);
	}

	mouse_x = new_x;
	mouse_y = new_y;
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (button != GLFW_MOUSE_BUTTON_LEFT) return;
	pressed = action == GLFW_PRESS;
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	camera.Scroll(-0.2 * yoffset);
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Maya", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);

	glEnable(GL_DEPTH_TEST);

	Model model("resources/models/batmobile", "batmobile.obj");
	Shader shader("shaders/maya.vs", "shaders/maya.fs");

	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		shader.SetUniform<mat4>("mvp", ViewProjection(camera.position(), center_position));
	
		model.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;
}

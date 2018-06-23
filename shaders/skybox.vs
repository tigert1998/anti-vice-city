#version 330 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 rotate;

layout (location = 0) in vec3 positions;

out vec3 tex_coordinate;

void main() {
	tex_coordinate = vec3(rotate * vec4(positions, 1));
	gl_Position = projection * view * vec4(positions, 1);
}
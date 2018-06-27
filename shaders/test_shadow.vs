#version 330 core

layout (location = 0) in vec3 positions;
layout (location = 1) in vec2 tex_coordinates;

out vec2 tex_coordinate;

void main() {
	gl_Position = vec4(positions, 1);
	tex_coordinate = tex_coordinates;
}
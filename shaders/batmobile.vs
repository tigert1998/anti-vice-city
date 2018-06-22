#version 330 core

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 tex_coordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 position;
out vec3 normal;
out vec2 tex_coordinate;

void main() {
	position = positions;
	normal = normals;
	tex_coordinate = tex_coordinates;

	gl_Position = projection * view * model * vec4(positions, 1);
}
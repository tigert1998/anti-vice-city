#version 330 core

layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 tex_coordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat4 view_projection_matrix;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

void main() {
	gl_Position = projection * view * model * vec4(positions, 1);

	Position = vec3(model * vec4(positions, 1.0f));;
	Normal = normalize(transpose(inverse(mat3(model))) * normals);
	TexCoord = tex_coordinates;
}
#version 330 core

struct Material {
	sampler2D texture_diffuse_0;
	sampler2D texture_diffuse_1;
	sampler2D texture_diffuse_2;

	sampler2D texture_specular_0;
	sampler2D texture_specular_1;
	sampler2D texture_specular_2;
};

uniform Material material;

out vec4 fragment_color;

in vec3 position;
in vec3 normal;
in vec2 tex_coordinate;

void main() {
	fragment_color = texture(material.texture_diffuse_0, tex_coordinate);
}
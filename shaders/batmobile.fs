#version 330 core

struct Material {
	sampler2D texture_diffuse_0;
	sampler2D texture_specular_0;
	sampler2D texture_normals_0;

	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 view_position;

out vec4 fragment_color;

in vec3 position;
in vec3 normal;
in vec2 tex_coordinate;

void main() {
	vec3 view_direction = normalize(view_position - position);
	vec3 light_direction = normalize(light.position - position);
	vec3 reflect_direction = normalize(reflect(-light_direction, normal));

	vec3 ambient = 
		light.ambient *
		texture(material.texture_diffuse_0, tex_coordinate).rgb;

	vec3 diffuse =
		light.diffuse *
		texture(material.texture_diffuse_0, tex_coordinate).rgb *
		max(0, dot(light_direction, normal));

	vec3 specular =
		light.specular *
		texture(material.texture_specular_0, tex_coordinate).rgb *
		pow(max(0, dot(reflect_direction, view_direction)), material.shininess);

	fragment_color = vec4(ambient + diffuse + specular, 1);
	// fragment_color = texture(material.texture_specular_0, tex_coordinate);
}
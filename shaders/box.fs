#version 330 core

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform vec3 view_position;
uniform Light light;
uniform Material material;

in vec2 tex_coordinate;
in vec3 frag_position;
in vec3 normal;

out vec4 frag_color;

void main() {
	vec3 ambient = light.ambient * texture(material.diffuse, tex_coordinate).rgb;

	vec3 light_direction = normalize(light.position - frag_position);
	vec3 diffuse = 
		light.diffuse * 
		texture(material.diffuse, tex_coordinate).rgb *
		max(dot(normalize(normal), light_direction), 0);

	vec3 view_direction = normalize(view_position - frag_position);
	vec3 reflect_direction = reflect(-light_direction, normal);
	vec3 specular = 
		light.specular * 
		texture(material.specular, tex_coordinate).rgb *
		pow(max(dot(view_direction, reflect_direction), 0), material.shininess);

	vec3 result = ambient + diffuse + specular;

	frag_color = vec4(result, 1);
	// frag_color = vec4(tex_coordinate, 0, 1);
}
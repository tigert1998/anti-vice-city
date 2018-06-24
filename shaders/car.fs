#version 330 core

struct Material {
	sampler2D texture_diffuse_0;
	sampler2D texture_specular_0;
	sampler2D texture_normals_0;
	sampler2D texture_ambient_0;

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

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

void main() {
	vec3 view_direction = normalize(view_position - Position);
	vec3 light_direction = normalize(light.position - Position);
	vec3 reflect_direction = normalize(reflect(-light_direction, Normal));

	vec3 ambient = 
		light.ambient *
		texture(material.texture_diffuse_0, TexCoord).rgb;

	vec3 diffuse =
		light.diffuse *
		texture(material.texture_diffuse_0, TexCoord).rgb *
		max(0.0, dot(light_direction, Normal));

	vec3 specular =
	    light.specular *
		texture(material.texture_specular_0, TexCoord).rgb *
		pow(max(0, dot(reflect_direction, view_direction)), material.shininess);

	gl_FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
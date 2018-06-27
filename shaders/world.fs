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

struct Shadow {
	sampler2D texture;
	mat4 view_projection_matrix;
};

uniform Shadow shadow;
uniform Material material;
uniform Light light;
uniform vec3 view_position;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

float CalcShadowCoefficient() {
    vec4 light_space_position = shadow.view_projection_matrix * vec4(Position, 1);
    light_space_position /= light_space_position.w;
    float depth = light_space_position.z * 0.5 + 0.5;
    float closest_depth = texture(shadow.texture, (vec2(light_space_position.xy) * 0.5 + 0.5)).r;
    // float bias = max(0.05 * (1.0 - dot(normal, light_direction)), 0.005);
    float bias = 0.005;
    return depth - bias > closest_depth ? 1.0f : 0.0f;
}

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

	float shadow_coefficient = CalcShadowCoefficient();

	gl_FragColor = vec4(ambient + shadow_coefficient * (diffuse + specular), 1.0f);
}
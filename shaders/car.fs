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
	vec3 light_direction;
};

uniform Shadow shadow;
uniform Material material;
uniform Light light;
uniform vec3 view_position;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

float CalcShadowCoefficient() {
	vec4 fragPosLightSpace = shadow.view_projection_matrix * vec4(Position, 1);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z;
	float closestDepth = texture(shadow.texture, projCoords.xy).r;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(shadow.light_direction);
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float result = 0.0;
	vec2 texelSize = 1.0f / textureSize(shadow.texture, 0);

	for(int x = -1; x <= 1; ++x)
	{
	    for(int y = -1; y <= 1; ++y)
	    {
	        float pcfDepth = texture(shadow.texture, projCoords.xy + vec2(x, y) * texelSize).r; 
	        result += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
	    }    
	}
	result /= 9.0;
    
	if(projCoords.z > 1.0)
	    result = 0.0;
	    
	return 1 - result;
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
#version 330 core

uniform sampler2D shadow_texture;

in vec2 tex_coordinate;

void main() {
	float value = texture(shadow_texture, tex_coordinate).r + 0.5;
	gl_FragColor = vec4(vec3(value), 1);
}
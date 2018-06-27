#version 330 core

uniform sampler2D shadow_texture;

in vec2 tex_coordinate;

void main() {
	float closest_depth = texture(shadow_texture, tex_coordinate).r;
	gl_FragColor = vec4(vec3(closest_depth), 1);
	// gl_FragColor = vec4(tex_coordinate, 0, 1 + 0.000001 * closest_depth);
}
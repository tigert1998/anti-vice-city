#version 330 core

uniform samplerCube skybox;

in vec3 tex_coordinate;

void main() {
	gl_FragColor = texture(skybox, tex_coordinate);
}
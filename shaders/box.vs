#version 330 core

uniform mat4 mvp;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 tex_coordinates;

out vec2 tex_coordinate;
out vec3 frag_position;
out vec3 normal;

void main()
{
    gl_Position = mvp * vec4(position.xyz, 1.0);
    tex_coordinate = tex_coordinates;
    frag_position = position;
    normal = normals;
}

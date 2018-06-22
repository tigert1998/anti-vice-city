#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 tex_coordinates;
layout (location = 3) in vec3 tangents;

out vec2 tex_coordinate;
out vec3 frag_position;
out vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(position.xyz, 1.0);
    gl_Position = mvp * vec4(position.xyz, 1.0);
    tex_coordinate = tex_coordinates;
    frag_position = position;
    normal = normals;

    mat3 normal_matrix = transpose(inverse(mat3(model)));
    vec3 t = normalize(normal_matrix * tangents);
}

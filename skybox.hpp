#pragma once

#include <vector>
#include <string>

#include "shader.hpp"
#include "opengl_util.hpp"
#include "camera.hpp"

class Skybox {
public:
	Skybox() = delete;
	Skybox(const std::vector<std::string> &urls, const Shader &shader, const Camera &camera);
	void Draw() const;

private:
	static const std::vector<float> vertices;
    const Shader &shader_;
    const Camera &camera_;
	uint32_t texture_, vao, vbo;
};

Skybox::Skybox(const std::vector<std::string> &urls, const Shader &shader, const Camera &camera): 
    shader_(shader),
    camera_(camera) {
	texture_ = LoadCubeMap(urls);
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);
}

void Skybox::Draw() const {
    using namespace glm;
    shader_.Use();
    shader_.SetUniform<mat4>("view", mat4(mat3(camera_.GetViewMatrix())));
    shader_.SetUniform<mat4>("projection", camera_.GetProjectionMatrix());
    shader_.SetUniform<mat4>("rotate", rotate(mat4(1), -(float)M_PI / 2, vec3(1, 0, 0)));

	glDepthMask(false);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
    shader_.SetUniform<int32_t>("skybox", 0);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDepthMask(true);
}

const std::vector<float> Skybox::vertices = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

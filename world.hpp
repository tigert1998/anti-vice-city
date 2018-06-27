#pragma once

#include "model.hpp"
#include "camera.hpp"
#include "shader.hpp"

class World {
public:
	World() = delete;
	World(const Model &model, const Shader &shader, const Camera &camera);
	void Draw() const;
	glm::mat4 model_matrix() const;
	const Model &model() const;
	const Shader &shader() const;

private:
	const Model &model_;
	const Shader &shader_;
	const Camera &camera_;
};

const Model &World::model() const {
	return model_;
}

const Shader &World::shader() const {
	return shader_;
}

glm::mat4 World::model_matrix() const {
	using namespace glm;
	return scale(mat4(1), vec3(50, 50, 50)) * rotate(mat4(1), (float)M_PI / 2, vec3(1, 0, 0));
}

World::World(const Model &model, const Shader &shader, const Camera &camera):
	model_(model),
	shader_(shader),
	camera_(camera) {
}

void World::Draw() const {
	using namespace glm;
	shader_.Use();
	shader_.SetUniform<mat4>("model", model_matrix());
	shader_.SetUniform<mat4>("view", camera_.GetViewMatrix());
	shader_.SetUniform<mat4>("projection", camera_.GetProjectionMatrix());	
	shader_.SetUniform<vec3>("light.position", vec3(200, 200, 500));
	shader_.SetUniform<vec3>("light.ambient", vec3(0.6, 0.6, 0.6));
	shader_.SetUniform<vec3>("light.diffuse", vec3(1, 1, 1));
	shader_.SetUniform<vec3>("light.specular", vec3(1, 1, 1));
	shader_.SetUniform<vec3>("view_position", camera_.position());
	shader_.SetUniform<float>("material.shininess", 32);
	model_.Draw(shader_, true);	
}
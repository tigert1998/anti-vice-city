#pragma once

#include "model.hpp"
#include "camera.hpp"

class Car {
private:
	const glm::vec3 up_ = glm::vec3(0, 0, 1);
	const glm::vec3 front_ = glm::vec3(1, 0, 0);
	const Model &model_;
	const Shader &shader_;
	Camera &camera_;
	glm::vec3 position_;
	double x_, y_;
	void CameraAccompany();

public:
	Car() = delete;
	Car(const Model &model, const Shader &shader, Camera &camera_ptr, glm::vec3);
	void Draw() const;
	void Move(MoveDirectionType, float time);
	glm::mat4 model_matrix() const;
};

glm::mat4 Car::model_matrix() const {
	using namespace glm;
	mat4 model = scale(rotate(mat4(1), (float)M_PI / 2, vec3(1, 0, 0)), vec3(0.0002, 0.0002, 0.0002));
	model = translate(mat4(1), position_) * model;
	return model;
}

Car::Car(const Model &model, const Shader &shader, Camera &camera, glm::vec3 position):
	model_(model),
	shader_(shader),
	camera_(camera),
	position_(position) {
	CameraAccompany();
}

void Car::Draw() const {
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

	model_.Draw(shader_);
}

void Car::CameraAccompany() {
	double x = position_.x, y = position_.y, z = position_.z;
	camera_.set_position(glm::vec3(x - 0.2, y, z + 0.1));
}

void Car::Move(MoveDirectionType direction, float time) {
	using namespace std;
	auto left = glm::cross(up_, front_);
	auto right = -left;
	switch (direction) {
		case MoveDirectionType::FRONT:
			position_ += front_ * time;
			break;
		case MoveDirectionType::BACK:
			position_ -= front_ * time;
			break;
		case MoveDirectionType::LEFT:
			position_ += left * time;
			break;
		case MoveDirectionType::RIGHT:
			position_ += right * time;
			break;
	}
	CameraAccompany();
}
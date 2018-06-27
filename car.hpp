#pragma once

#include "model.hpp"
#include "camera.hpp"

#include <iostream>

class Car {
private:
	const glm::vec3 up_ = glm::vec3(0, 0, 1);
	glm::vec3 front_;
	const Model &model_;
	const Shader &shader_;
	Camera &camera_;
	glm::vec3 position_;

	double alpha_;
	bool motion_;

public:
	Car() = delete;
	Car(const Model &model, const Shader &shader, Camera &camera_ptr, glm::vec3);
	void Draw() const;
	void Move(MoveDirectionType, float time);
	glm::mat4 model_matrix() const;

	void CameraAccompany();
	void Rotate(double delta_alpha);
	const Model &model() const;
	const Shader &shader() const;
	void Enable();
	void Disable();
};

const Shader &Car::shader() const {
	return shader_;
}

const Model &Car::model() const {
	return model_;
}

glm::mat4 Car::model_matrix() const {
	using namespace glm;
	mat4 model(1);
	model = rotate(model, (float)M_PI / 2, vec3(1, 0, 0));
	model = scale(model, vec3(0.0002, 0.0002, 0.0002));
	model = rotate(model, (float)this->alpha_, vec3(0, 1, 0));
	model = translate(mat4(1), position_) * model;

	// std::cout << position_.x << " " << position_.y << " " << position_.z << std::endl;

	return model;
}

Car::Car(const Model &model, const Shader &shader, Camera &camera, glm::vec3 position):
	alpha_(0.0),
	motion_(true),
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

	model_.Draw(shader_, true);
}

void Car::CameraAccompany() {
	// double x = position_.x, y = position_.y, z = position_.z;
	// camera_.set_position(position_ + glm::vec3(-0.2, 0, 0.1));
	camera_.set_position(position_ - 0.1f * front_ + glm::vec3(0, 0, 0.1));
	// camera_.set_position(position_ - 0.1f * front_);
}

void Car::Move(MoveDirectionType direction, float time) {
	static MoveDirectionType last_dir;
	if (!this->motion_ && direction == last_dir) {
		return;
	}

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
		case MoveDirectionType::UP:
			position_ += up_ * time;
			break;
		case MoveDirectionType::DOWN:
			position_ -= up_ * time;
			break;
	}
	CameraAccompany();

	last_dir = direction;
}

void Car::Rotate(double delta_alpha) {
	this->alpha_ += delta_alpha;
	this->front_ = glm::vec3(cos(this->alpha_), sin(this->alpha_), 0);
}

void Car::Enable()
{
	this->motion_ = true;
}

void Car::Disable()
{
	this->motion_ = false;
}

#pragma once

#include "model.hpp"
#include "camera.hpp"

#ifdef DEBUG
#include <set>
#endif

#include <iostream>

class Car {
private:
	const glm::vec3 up_ = glm::vec3(0, 0, 1);
	// const glm::vec3 front_ = glm::vec3(1, 0, 0);
	glm::vec3 front_;
	const Model &model_;
	const Shader &shader_;
	Camera &camera_;
	glm::vec3 position_;

#ifdef DEBUG
	std::set< std::pair< std::pair<float, float>, std::pair<float, float> > > boxes_;
#endif

	double alpha_;
	bool motion_;
	float z_velocity_;
	bool fall_;
	const float gravity_ = 1.0f;

public:
	Car() = delete;
	Car(const Model &model, const Shader &shader, Camera &camera_ptr, glm::vec3);
	void Draw() const;
	void Move(MoveDirectionType, float time);
	glm::mat4 model_matrix() const;

	void CameraAccompany();
	void Rotate(double delta_alpha);
	void Enable();
	void Disable();


	void Update(float time);
	// void ResetVelocity();
	void ConflictZ();

#ifdef DEBUG
	void ShowPosition();
#endif
};

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
	fall_(true),
	alpha_(0.0),
	z_velocity_(0.0f),
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

	model_.Draw(shader_);
}

void Car::CameraAccompany() {
	// double x = position_.x, y = position_.y, z = position_.z;
	// camera_.set_position(position_ + glm::vec3(-0.2, 0, 0.1));
	camera_.set_position(position_ - 0.3f * front_ + glm::vec3(0, 0, 0.1));
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

void Car::Update(float time)
{
	this->ConflictZ();
	if (fall_) {
		this->position_.z -= time * this->z_velocity_;
		this->z_velocity_ += time * this->gravity_;

		this->ConflictZ();

		if (!fall_) {
			this->position_.z += time * this->z_velocity_ - 0.01f;
			this->z_velocity_ = 0.0f;
		}
	}
}

// void Car::ResetVelocity()
// {
// 	this->z_velocity_ = 0.0f;
// }

void Car::ConflictZ()
{
	using glm::vec3;

	const BoundingBox boxes[] = {
		BoundingBox(vec3(-8.9, -3, 4.5), vec3(-2, 9.6, 4.845)),
		BoundingBox(vec3(-2.2, 0, 4.5), vec3(1.8, 12.25, 4.845)),
		BoundingBox(vec3(-2.2, 12.3, 4.2), vec3(1.8, 13.6, 4.62)),
		BoundingBox(vec3(-3.26, 13.7, 2.7), vec3(1.8, 21.9, 3.16))
	};

	glm::vec3 pos_rotate = glm::rotate(glm::mat4(1), (float)M_PI / 4, glm::vec3(0, 0, 1)) * glm::vec4(position_, 0.0f);
	fall_ = true;
	for (const auto & box: boxes) {
		fall_ = fall_ &&  !box.InBox(pos_rotate);
	}
}

#ifdef DEBUG
void Car::ShowPosition()
{
// 	float px = position_.x;
// 	float py = position_.y;

// 	glm::vec3 after = glm::rotate(glm::mat4(1), (float)M_PI / 4, glm::vec3(0, 0, 1)) * glm::vec4(px, py, 0.0f, 0.0f);
// 	px = after.x;
// 	py = after.y;

// 	std::cout << px << " " << py << std::endl;
// 	std::cout << position_.x << " " << position_.y << " " << position_.z << std::endl;
}
#endif

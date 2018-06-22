#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class MoveDirectionType {
	FRONT, BACK, LEFT, RIGHT
};

class Camera {
public:
	Camera() = delete;
	Camera(glm::vec3 position, double alpha, double beta);
	void Rotate(double delta_alpha, double delta_beta);
	void Move(MoveDirectionType, float time);
	glm::vec3 position() const;
	glm::mat4 GetViewMatrix() const;

private:
	const glm::vec3 up_ = glm::vec3(0, 0, 1);
	glm::vec3 position_;
	double alpha_, beta_;
	glm::vec3 front() const;

};

Camera::Camera(glm::vec3 position, double alpha, double beta) {
	position_ = position;
	alpha_ = alpha;
	beta_ = beta;
}

void Camera::Rotate(double delta_alpha, double delta_beta) {
	alpha_ += delta_alpha;
	beta_ += delta_beta;
	beta_ = std::min(static_cast<double>(5 * M_PI / 12), beta_);
	beta_ = std::max(static_cast<double>(-5 * M_PI / 12), beta_);
}

glm::vec3 Camera::position() const {
	return position_;
}

glm::vec3 Camera::front() const {
	return glm::vec3(cos(alpha_) * cos(beta_), sin(alpha_) * cos(beta_), sin(beta_));
}

void Camera::Move(MoveDirectionType direction, float time) {
	auto left = glm::cross(up_, front());
	auto right = -left;
	switch (direction) {
		case MoveDirectionType::FRONT:
			position_ += front() * time;
			break;
		case MoveDirectionType::BACK:
			position_ -= front() * time;
			break;
		case MoveDirectionType::LEFT:
			position_ += left * time;
			break;
		case MoveDirectionType::RIGHT:
			position_ += right * time;
			break;
	}
}

glm::mat4 Camera::GetViewMatrix() const {
	return glm::lookAt(position_, position_ + front(), up_);
}
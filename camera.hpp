#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	Camera() = delete;
	Camera(double radius);
	
	double radius() const;
	glm::vec3 position() const;

	void Rotate(double delta_alpha, double delta_beta);	
	void Scroll(double delta_radius);


private:
	double alpha, beta, radius_;
	glm::vec3 position_;

	void Update();

};

Camera::Camera(double radius) {
	radius_ = radius;
	Update();
}

void Camera::Rotate(double delta_alpha, double delta_beta) {
	alpha += delta_alpha;
	beta += delta_beta;
	beta = std::min(static_cast<double>(5 * M_PI / 12), beta);
	beta = std::max(static_cast<double>(-5 * M_PI / 12), beta);
	Update();
}

void Camera::Scroll(double delta_radius) {
	radius_ += delta_radius;
	radius_ = std::max(0.1, radius_);
	Update();
}

double Camera::radius() const {
	return radius_;
}

glm::vec3 Camera::position() const {
	return position_;
}

void Camera::Update() {
	using namespace glm;
	position_ = vec3(radius_ * cos(beta) * cos(alpha), radius_ * cos(beta) * sin(alpha), radius_ * sin(beta));
}
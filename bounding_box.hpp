#pragma once

#include <string>
#include <vector>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#include "opengl_util.hpp"
#include "mesh.hpp"

// AABB BoundingBox
class BoundingBox
{
private:
	glm::vec3 big, small;
	uint32_t vao, vbo, ebo;
	glm::vec3 vertices[8];
	const GLushort indices[24] = { 0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6 ,7 };
	float GetVolumn() const;

public:
	BoundingBox(const Mesh &mesh);
	virtual ~BoundingBox();

	bool InBox(glm::vec3 position) const;
	void InitDraw();
	void Draw() const;
	void Merge(const BoundingBox &);

	bool Conflict(const BoundingBox &box, glm::mat4 transform_from_b_to_a, glm::mat4 transform_from_a_to_b) const;
};

bool BoundingBox::Conflict(const BoundingBox &box, glm::mat4 transform_from_b_to_a, glm::mat4 transform_from_a_to_b) const {
	const float bound = 0.006;
	if (box.GetVolumn() > bound) {
		return false;
	}

	using namespace glm;
	for (vec3 vertex: box.vertices) {
		vertex = transform_from_b_to_a * vec4(vertex, 1);
		if (this->InBox(vertex)) {
// #ifdef DEBUG
// 			std::cout << this->GetVolumn() << std::endl;
// #endif
			return true;
		}
	}
	for (vec3 vertex: vertices) {
		vertex = transform_from_a_to_b * vec4(vertex, 1);
		if (box.InBox(vertex)) {
// #ifdef DEBUG
// 			std::cout << box.GetVolumn() << std::endl;
// #endif
			return true;
		}
	}
	return false;
}

BoundingBox::~BoundingBox()
{
	//
}

BoundingBox::BoundingBox(const Mesh & mesh)
{
	using glm::vec3;

	bool first = true;

	const auto & vertices = mesh.GetVertices();
	for (const auto & vertex : vertices) {
		const auto & p = vertex.position;
		if (first) {
			first = false;
			this->big = p;
			this->small = p;
		} else {
			this->big.x = std::max(this->big.x, p.x);
			this->big.y = std::max(this->big.y, p.y);
			this->big.z = std::max(this->big.z, p.z);

			this->small.x = std::min(this->small.x, p.x);
			this->small.y = std::min(this->small.y, p.y);
			this->small.z = std::min(this->small.z, p.z);
		}
	}

	this->vertices[0] = vec3(this->small.x, this->small.y, this->small.z);
	this->vertices[1] = vec3(this->big.x,   this->small.y, this->small.z);
	this->vertices[2] = vec3(this->small.x, this->big.y,   this->small.z);
	this->vertices[3] = vec3(this->big.x,   this->big.y,   this->small.z);
	this->vertices[4] = vec3(this->small.x, this->small.y, this->big.z);
	this->vertices[5] = vec3(this->big.x,   this->small.y, this->big.z);
	this->vertices[6] = vec3(this->small.x, this->big.y,   this->big.z);
	this->vertices[7] = vec3(this->big.x,   this->big.y,   this->big.z);
}

bool BoundingBox::InBox(glm::vec3 position) const {
    return small.x <= position.x && position.x <= big.x &&
           small.y <= position.y && position.y <= big.y &&
           small.z <= position.z && position.z <= big.z;
}

float BoundingBox::GetVolumn() const
{
	float dx = this->big.x - this->small.x;
	float dy = this->big.y - this->small.y;
	float dz = this->big.z - this->small.z;

	return dx * dy * dz;
}

void BoundingBox::Merge(const BoundingBox & box)
{
	this->small.x = std::min(this->small.x, box.small.x);
	this->small.y = std::min(this->small.y, box.small.y);
	this->small.z = std::min(this->small.z, box.small.z);
	this->big.x = std::max(this->big.x, box.big.x);
	this->big.y = std::max(this->big.y, box.big.y);
	this->big.z = std::max(this->big.z, box.big.z);
}

#ifdef DEBUG

void BoundingBox::InitDraw()
{
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ebo);

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->vertices), this->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->indices), this->indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), (GLvoid*)0);

	glBindVertexArray(0);
}

void BoundingBox::Draw() const
{
	glBindVertexArray(this->vao);
	// 12 lines
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

#endif

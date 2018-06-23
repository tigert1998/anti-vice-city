#pragma once

#include <string>
#include <vector>

#include "opengl_util.hpp"
#include "mesh.hpp"

// AABB BoundingBox
class BoundingBox
{
private:
	glm::vec3 big, small;
	glm::mat4 model;

	uint32_t vao, vbo, ebo;
	glm::vec3 vertices[8];
	const GLushort indices[24] = { 0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6 ,7 };

public:
	BoundingBox(const Mesh & mesh);
	virtual ~BoundingBox();

	void SetModel(glm::mat4 model);
	bool InBox(glm::vec3 position);
	void InitDraw();
	void Draw() const;
};

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

void BoundingBox::SetModel(glm::mat4 model)
{
	this->model = model;
}

bool BoundingBox::InBox(glm::vec3 position)
{
	using glm::vec4;

	vec4 real_big = model * vec4(big, 0);
	vec4 real_small = model * vec4(small, 0);

    return real_small.x <= position.x && position.x <= real_big.x &&
           real_small.y <= position.y && position.y <= real_big.y &&
           real_small.z <= position.z && position.z <= real_big.z;
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

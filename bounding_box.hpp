#pragma once

#include <string>
#include <vector>

#include "opengl_util.hpp"
#include "mesh.hpp"


class BoundingBox
{
private:
	glm::vec3 big, small;
	glm::mat4 model;

public:
	BoundingBox(const Mesh & mesh);
	virtual ~BoundingBox();

	void SetModel(glm::mat4 model);
	bool InBox(glm::vec3 position);
};

BoundingBox::~BoundingBox()
{
	//
}

BoundingBox::BoundingBox(const Mesh & mesh)
{
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

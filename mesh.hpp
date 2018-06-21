#pragma once

#include <vector>
#include <string>

#include "shader.hpp"

enum class TextureType {
	DIFFUSE, SPECULAR
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coordinate;
};

struct Texture {
	uint32_t id;
	TextureType type;
};

class Mesh {
public:
	Mesh() = delete;
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures);
	void Draw(Shader shader) const;

private:
	uint32_t vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	void InitMesh();
};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	InitMesh();
}

void Mesh::InitMesh() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void *)offsetof(Vertex, tex_coordinate));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader shader) const {
	using namespace std;
	uint32_t diffuse_total = 0, specular_total = 0;
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		string identifier;
		switch (textures[i].type) {
			case TextureType::DIFFUSE:
				identifier = "material.texture_diffuse_" + to_string(diffuse_total++);
				break;
			case TextureType::SPECULAR:
				identifier = "material.texture_specular_" + to_string(specular_total++);
				break;
		}
		shader.SetUniform<int32_t>(identifier, i);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
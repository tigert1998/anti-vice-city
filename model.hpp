#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "cg_exception.hpp"
#include "opengl_util.hpp"
#include "bounding_box.hpp"

class Model {
private:
	std::string path;
	std::vector<Mesh> meshes_;
	std::vector<BoundingBox> boxes;

	void DFSNode(aiNode *, const aiScene *);
	Mesh DealMesh(aiMesh *, const aiScene *);
	std::vector<Texture> LoadMaterialTextures(aiMaterial *, aiTextureType);

public:
	Model() = delete;
	Model(const std::string &, const std::string &);

	void Draw(Shader) const;
	const std::vector<Mesh> &meshes() const;
	bool Conflict(const Model &model, glm::mat4 a_model_matrix, glm::mat4 b_model_matrix) const;
};

bool Model::Conflict(const Model &model, glm::mat4 a_model_matrix, glm::mat4 b_model_matrix) const {
	glm::mat4 transform_from_b_to_a = inverse(a_model_matrix) * b_model_matrix;
	glm::mat4 transform_from_a_to_b = inverse(b_model_matrix) * a_model_matrix;
	for (const BoundingBox &a_box: this->boxes)
		for (const BoundingBox &b_box: model.boxes) 
			if (a_box.Conflict(b_box, transform_from_b_to_a, transform_from_a_to_b))
				return true;
	return false;
}

const std::vector<Mesh> &Model::meshes() const {
	return meshes_;
}

Model::Model(const std::string &path, const std::string &file): path(path) {
	using namespace Assimp;
	using namespace std;
	Importer importer;
	auto scene = importer.ReadFile(path + "/" + file, 
		aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_CalcTangentSpace
	);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		throw AssimpError(importer.GetErrorString());
	}
	DFSNode(scene->mRootNode, scene);
}

void Model::Draw(Shader shader) const {
	for (const Mesh &i : meshes_)
		i.Draw(shader);

#ifdef DEBUG
	for (const BoundingBox & box : boxes) {
		box.Draw();
	}
#endif
}

void Model::DFSNode(aiNode *node, const aiScene *scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh m = DealMesh(mesh, scene);
		meshes_.push_back(m);

		// bounding box
		BoundingBox box(m);
#ifdef DEBUG
		box.InitDraw();
#endif
		boxes.push_back(box);
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		DFSNode(node->mChildren[i], scene);
	}
}

Mesh Model::DealMesh(aiMesh *mesh, const aiScene *scene) {
	using namespace std;
	using namespace glm;
	vector<Vertex> vertices;
	vector<uint32_t> indices;
	vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->HasNormals())
			vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->HasTextureCoords(0))
			vertex.tex_coordinate = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertex.tangent = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		vertices.push_back(vertex);
	}
	for (int i = 0; i < mesh->mNumFaces; i++) {
		for (int j = 0; j < 3; j++) {
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuse_textures = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
		vector<Texture> specular_textures = LoadMaterialTextures(material, aiTextureType_SPECULAR);
		vector<Texture> normals_textures = LoadMaterialTextures(material, aiTextureType_NORMALS);
		vector<Texture> ambient_textures = LoadMaterialTextures(material, aiTextureType_AMBIENT);
		copy(diffuse_textures.begin(), diffuse_textures.end(), back_inserter(textures));
		copy(specular_textures.begin(), specular_textures.end(), back_inserter(textures));
		copy(normals_textures.begin(), normals_textures.end(), back_inserter(textures));
		copy(ambient_textures.begin(), ambient_textures.end(), back_inserter(textures));
	}
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial *material, aiTextureType type) {
	using namespace std;	
	vector<Texture> textures;
	for (uint32_t i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		Texture texture;
		texture.id = LoadTexture(path + "/" + str.C_Str());
		switch (type) {
			case aiTextureType_DIFFUSE:
				texture.type = TextureType::DIFFUSE;
				break;
			case aiTextureType_SPECULAR:
				texture.type = TextureType::SPECULAR;
				break;
			case aiTextureType_NORMALS:
				texture.type = TextureType::NORMALS;
				break;
			case aiTextureType_AMBIENT:
				texture.type = TextureType::AMBIENT;
				break;
			default:
				break;
		}
		textures.push_back(texture);
	}
	return textures;
}

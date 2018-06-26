#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.hpp"
#include "shader.hpp"
#include "opengl_util.hpp"
#include "constant.hpp"

class Shadow {
public:
	Shadow() = delete;
	Shadow(const Shader &shader, glm::mat4 view_projection_matrix);
	void Render(std::vector<const Model *> model_ptrs, std::vector<glm::mat4> model_matrices) const;
	uint32_t texture() const;

private:
	const Shader &shader;
	uint32_t fbo, texture_;
	glm::mat4 view_projection_matrix;

};

Shadow::Shadow(const Shader &shader, glm::mat4 view_projection_matrix):
	shader(shader),
	view_projection_matrix(view_projection_matrix) {
	texture_ = GenerateDepthTexture(SHADOW_WIDTH, SHADOW_HEIGHT);
	fbo = GenerateFrameBuffer(SHADOW_WIDTH, SHADOW_HEIGHT, texture_);
}

void Shadow::Render(std::vector<const Model *> model_ptrs, std::vector<glm::mat4> model_matrices) const {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_DEPTH_BUFFER_BIT);

	shader.Use();
	shader.SetUniform<glm::mat4>("light_space_matrix", view_projection_matrix);
	
	for (int i = 0; i < model_ptrs.size(); i++) {
		auto model_ptr = model_ptrs[i];
		auto model_matrix = model_matrices[i];
		shader.SetUniform<glm::mat4>("model", model_matrix);
		model_ptr->Draw(shader);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

uint32_t Shadow::texture() const {
	return texture_;
}
#pragma once

#include <string>
#include <vector>

#include "opengl_util.hpp"
#include "shader_manager.hpp"


class SkyboxManager {
public:
	SkyboxManager() = delete;
	SkyboxManager(vector<std::string>, const std::string &, const std::string &);

private:
	const ShaderManager &shader_manager = ShaderManager::shared;
	uint32_t texture, program;

};

SkyboxManager::SkyboxManager(vector<std::string> urls, const std::string &vertex_shader_path, const std::string &fragment_shader_path) {
	texture = LoadCubeMap(urls);
	program = shader_manager.CompileFilesAt(vertex_shader_path, fragment_shader_path);
}
